#include "stStitchingParams.h"

//
// OMP was previously used to enhance computation speed.
// I found that my innermost "for" loop was referencing variables
// outside the loops that were changed per the loop's current
// iteration values. The problem seemed to be that the paralell
// processing was moving out of sync with the modified variables,
// so on iteration 1 and iteration 2 in paralell, variable 1 and
// variable 2 were accidentally used by both. So for example,
// when the variable is the wdwTrLmt, and the sizes of the images
// are different, then these are not interchangeable and will
// cause an out of bounds indexing. The computation benefit was
// not worthwhile at the time of writing this, to warrant a huge
// modification, as seemed necessary to get it working.
//

void stAffineParamSearch( const double* tformLimits,
                          const cv::Mat& imDoubleA,
                          const cv::Mat& imDoubleB,
                          const int& numEdge,
                          cv::Mat& tformOptimal,
                          double* tformOptimalParam,
                          int* wdwTrLmt,
                          cv::Point& maxMutInfoXY,
                          double& maxMutInfo )
{
    // ************************************ //
    int imRows = imDoubleA.rows;            // Total image rows
    int imCols = imDoubleA.cols;            // Total image cols

    int wdwTrLmtOrig[4] = { wdwTrLmt[0],    // Row Lower Bound (Translation Search Limits)
                            wdwTrLmt[1],    // Col Lower Bound
                            wdwTrLmt[2],    // Row Upper Bound
                            wdwTrLmt[3] };  // Col Upper Bound


    // ******************************** //
    double scaleFrst = tformLimits[0];  // Lower Scale Bound
    double scaleLast = tformLimits[1];  // Upper Scale Bound
    double scaleStep = tformLimits[2];  // Scale stepsize

    double skewRFrst = tformLimits[3];  // Lower Skew Bound
    double skewRLast = tformLimits[4];  // Upper Skew Bound
    double skewRStep = tformLimits[5];  // Skew stepsize

    double skewCFrst = tformLimits[6];  // Lower Skew Bound
    double skewCLast = tformLimits[7];  // Upper Skew Bound
    double skewCStep = tformLimits[8];  // Skew stepsize

    double rotFrst = tformLimits[9];    // Lower Rotation Bound (Degrees)
    double rotLast = tformLimits[10];   // Upper Rotation Bound (Degrees)
    double rotStep = tformLimits[11];   // Rotation stepsize (Degrees)


    // **************************************************** //
    double* ptrTformOptimal = (double*)tformOptimal.data;   // Pointer to resultant transform data
    cv::Size imSize = imDoubleA.size();                     // Original Image Size
    cv::Mat tformA = cv::Mat::eye( 3, 3, CV_64F );          // Initialize Transform Array (Identity Matrix)
    cv::Mat tformB = cv::Mat::eye( 3, 3, CV_64F );          // Initialize Transform Array (Identity Matrix)

    double* ptrTformA = (double*)tformA.data;               // Pointer to transform data
    double* ptrTformB = (double*)tformB.data;               // Pointer to transform data

    // Affine Search
    // Could be re-written as sequential, instead of nested
    // Would be limiting, but would save computation time
    // Could be helped out by projective match
    // (Rotation then Scale were found to be most important)
    // **************************************************** //
    for( double SCL = scaleFrst; SCL <= scaleLast; SCL += scaleStep )
    {
        for( double SKR = skewRFrst; SKR <= skewRLast; SKR += skewRStep )
        {
            for( double SKC = skewCFrst; SKC <= skewCLast; SKC += skewCStep )
            {
                for( double ROT = rotFrst; ROT <= rotLast; ROT += rotStep )
                {
                    double ROTRAD = ROT * PI / 180.0;

                    // PARAMETER DECLARATION AND SETUP
                    // ******************************************** // Transform Array:
                    ptrTformA[0] = SCL * cos( ROTRAD );             // a00
                    ptrTformA[1] = SKC * (sin( ROTRAD ));           // a01
                    ptrTformA[2] = 0;                               // a02
                    ptrTformA[3] = SKR  * (-sin( ROTRAD ));         // a10
                    ptrTformA[4] = SCL  * cos( ROTRAD );            // a11
                    ptrTformA[5] = 0;                               // a12

                    cv::Size imTformSize;                           // Transformed image size
                    cv::Point mutInfoXY;                            // Location of maximum of mutual information
                    double mutInfo;                                 // Maximum value of mutual information
                    int TRR;                                        // Row Translation parameter
                    int TRC;                                        // Col Translation parameter


                    // CENTERED AND SIZE CORRECTED IMAGE TRANSFORMATIONS
                    // ************************************************************ //
                    stTformSize( imSize,                                            // Input original image size
                                 imTformSize,                                       // Return new image size
                                 tformA );                                          // Input image transform

                    cv::Size imTformSizeOld = imTformSize;                          // Store Size as Old Size

                    imTformSize.width = ( imTformSizeOld.width > imSize.width )     // Is new size width greater than original width?
                                        ? imTformSizeOld.width                      // Store new if true
                                        : imSize.width;                             // Store original if false

                    imTformSize.height = ( imTformSizeOld.height > imSize.height )  // Is new size width greater than original height?
                                         ? imTformSizeOld.height                    // Store new if true
                                         : imSize.height;                           // Store original if false


                    // CENTERED AND SIZE CORRECTED IMAGE TRANSFORMATIONS
                    // ******************************************************** //
                    cv::Mat imCenterA = cv::Mat::zeros( imTformSize, CV_32F );  // Storage for centered, transformed gradient
                    cv::Mat imCenterB = cv::Mat::zeros( imTformSize, CV_32F );  // Storage for centered, transformed gradient


                    // CENTERED AND SIZE CORRECTED IMAGE TRANSFORMATIONS
                    // ************************************************************************ //
                    ptrTformA[2] += ROUND( (imTformSize.width - imTformSizeOld.width) / 2. );   // Add Centering Col Translation to Image A's Col Translation Parameter
                    ptrTformA[5] += ROUND( (imTformSize.height - imTformSizeOld.height) / 2. ); // Add Centering Row Translation to Image A's Row Translation Parameter

                    ptrTformB[2] = ROUND( (imTformSize.width - imDoubleB.cols) / 2. );          // Set Centering Col Translation for Image B
                    ptrTformB[5] = ROUND( (imTformSize.height - imDoubleB.rows) / 2. );         // Set Centering Row Translation for Image B


                    cv::warpPerspective( imDoubleA,                                             // Source Image
                                         imCenterA,                                             // Transformed Image
                                         tformA,                                                // Image Transform
                                         imTformSize,                                           // Size of Transformed image
                                         cv::INTER_AREA,                                        // Cubic Interpolation best for Scaling
                                         cv::BORDER_TRANSPARENT );                              // Transparent Border means Overlay

                    cv::warpPerspective( imDoubleB,                                             // Source Image
                                         imCenterB,                                             // Transformed Image
                                         tformB,                                                // Image Transform
                                         imTformSize,                                           // Size of Transformed image
                                         cv::INTER_CUBIC,                                       // Cubic Interpolation best for Scaling
                                         cv::BORDER_TRANSPARENT );                              // Transparent Border means Overlay


                    // Create and transform Mask to cover only valid centered image
                    // ******************************************************** //
                    cv::Mat tempA = cv::Mat::ones( imDoubleA.size(), CV_8U );   // Initial Mask
                    cv::Mat tempB = cv::Mat::ones( imDoubleB.size(), CV_8U );   // Initial Mask

                    cv::Mat maskCtrA = cv::Mat::zeros( imTformSize, CV_8U );    // Transformed Mask
                    cv::Mat maskCtrB = cv::Mat::zeros( imTformSize, CV_8U );    // Transformed Mask

                    cv::warpPerspective( tempA,                                 // Input to be Transformed
                                         maskCtrA,                              // Result
                                         tformA,                                // Transform array
                                         imTformSize,                           // Resultant Size
                                         cv::INTER_LINEAR,                      // Interpolation Method
                                         cv::BORDER_TRANSPARENT );              // What to do out of bounds (nothing, just overlay)

                    cv::warpPerspective( tempB,                                 // Input to be Transformed
                                         maskCtrB,                              // Result
                                         tformB,                                // Transform array
                                         imTformSize,                           // Resultant Size
                                         cv::INTER_LINEAR,                      // Interpolation Method
                                         cv::BORDER_TRANSPARENT );              // What to do out of bounds (nothing, just overlay)


                    uchar* ptrMaskCtrA = maskCtrA.data;                         // Pointer to mask data
                    uchar* ptrMaskCtrB = maskCtrB.data;                         // Pointer to mask data


                    // Gradient Map Normalization for Edge Quantization
                    // ************************************************ //
                    cv::Mat imGradA( imCenterA.size(), CV_8U );         // Create 8-bit storage for quantization
                    cv::Mat imGradB( imCenterA.size(), CV_8U );         // Create 8-bit storage for quantization

                    float* ptrImCenterA = (float*)imCenterA.data;       // Pointer to data in A
                    float* ptrImCenterB = (float*)imCenterB.data;       // Pointer to data in B

                    double minA, maxA;                                  // Image A extrema storage
                    double minB, maxB;                                  // Image B extrema storage

                    cv::minMaxLoc( imCenterA,                           // Image to find extremes in
                                   &minA,                               // minimum value
                                   &maxA,                               // maximum value
                                   0,                                   // location of minimum
                                   0,                                   // location of maximum
                                   maskCtrA );                          // mask

                    cv::minMaxLoc( imCenterB,                           // Image to find extremes in
                                   &minB,                               // minimum value
                                   &maxB,                               // maximum value
                                   0,                                   // location of minimum
                                   0,                                   // location of maximum
                                   maskCtrB );                          // mask


                    // Quantize to Integers in [1,255] to allow use of 0s as "no data" in Mut. Info. calculations
                    // ************************************************************************************ //
                    for( int i = 0; i < imRows*imCols; i++ )                                                // Normalize Map by Maximum and
                    {
                        if( ptrMaskCtrA[i] )                                                                // Only Valid within mask
                        {
                            ptrImCenterA[i] = (float)ROUND( 255.*((ptrImCenterA[i]-minA)/(maxA-minA)) );    // Normalize to [0,255]
                            ptrImCenterA[i] = ( ptrImCenterA[i] == 0 )                                      // Does Value Equal 0?
                                              ? 1                                                           // Set to 1 if true
                                              : ptrImCenterA[i];                                            // Leave as is, if false
                        }

                        if( ptrMaskCtrB[i] )                                                                // Only valid within mask
                        {
                            ptrImCenterB[i] = (float)ROUND( 255.*((ptrImCenterB[i]-minB)/(maxB-minB)) );    // Normalize to [0,255]
                            ptrImCenterB[i] = ( ptrImCenterB[i] == 0 )                                      // Does Value Equal 0?
                                              ? 1                                                           // Set to 1 if true
                                              : ptrImCenterB[i];                                            // Leave as is, if false
                        }
                    }


                    //  Quantize Normalized Images to 8-bit Arrays
                    // ******************************************** //
                    imCenterA.convertTo(imGradA, CV_8U, 1, 0 );     //
                    imCenterB.convertTo(imGradB, CV_8U, 1, 0 );     //


                    // Quantize Gradients to Edge Maps
                    // ******************************************************** //
                    cv::Mat imEdgeA = cv::Mat::zeros( imTformSize, CV_8U );     // Storage for centered, transformed, quantized gradient
                    cv::Mat imEdgeB = cv::Mat::zeros( imTformSize, CV_8U );     // Storage for centered, transformed, quantized gradient

                    stGradToEdge( imGradA,                                      // Gradient Image
                                  numEdge,                                      // Number of Desired Edges
                                  imEdgeA );                                    // Edge Image

                    stGradToEdge( imGradB,                                      // Gradient Image
                                  numEdge,                                      // Number of Desired Edges
                                  imEdgeB );                                    // Edge Image


                    // EXHAUSTIVE TRANSLATION SEARCH
                    // Use [a,b,0,0] case to search entire
                    // set of overlaps
                    // ******************************** //
                    int normSize = 0;                   // Create Storage for Normalization Factor

                    wdwTrLmt[2] = wdwTrLmt[2] == 0      // Is Col Upper Bound 0?
                                  ? 2*imEdgeA.rows-1    // If so, then set to Max Column Value
                                  : wdwTrLmt[2];        // Otherwise, keep it as is.

                    wdwTrLmt[3] = wdwTrLmt[3] == 0      // Is Col Upper Bound 0?
                                  ? 2*imEdgeA.cols-1    // If so, then set to Max Column Value
                                  : wdwTrLmt[3];        // Otherwise, keep it as is.

                    stTranslateSearch( imEdgeA,         // Input A
                                       imEdgeB,         // Input B
                                       numEdge,         // Total Number of Levels in Input Images
                                       normSize,        // Return: Normalization Factor
                                       wdwTrLmt,        // Array of Search Bounds
                                       TRR,             // Return: Row Translation
                                       TRC,             // Return: Col Translation
                                       mutInfoXY,       // Return: Mutual Info. Location
                                       mutInfo );       // Return: Mutual Info. Value


                    // When a maximum is found, store the values to return
                    // ************************************************ //
                    if( maxMutInfo < mutInfo )                          // If Current Value is > Current Maximum
                    {                                                   // Affine Transform:
                        ptrTformOptimal[0] = SCL * cos( ROTRAD );       // a00              | a00 a01 a02 |   | x |
                        ptrTformOptimal[1] = SKC * sin( ROTRAD );       // a01              | a10 a11 a12 | * | y |
                        ptrTformOptimal[2] = 0;                         // a02              | a20 a21 a22 |   | 1 |
                        ptrTformOptimal[3] = SKR  * (-sin( ROTRAD ));   // a10
                        ptrTformOptimal[4] = SCL  * cos( ROTRAD );      // a11
                        ptrTformOptimal[5] = 0;                         // a12

                        tformOptimalParam[0] = SCL;                     // Store Scale Factor
                        tformOptimalParam[1] = SKR;                     // Store Row Skew
                        tformOptimalParam[2] = SKC;                     // Store Column Skew
                        tformOptimalParam[3] = ROTRAD;                  // Store Rotation in Radians
                        tformOptimalParam[4] = TRR;                     // Store Row Translation
                        tformOptimalParam[5] = TRC;                     // Store Col Translation

                        maxMutInfo = mutInfo;                           // Update Maximum Mutual Info. Value
                        maxMutInfoXY = mutInfoXY;                       // Update Maximum Mutual Info. Location
                    }


                    // Reset wdwTrLmt back to original values in case it
                    // was modified to fit the transformed image limits
                    // (using [0,0,0,0] state as "full image" flag)
                    // ************************************************ //
                    wdwTrLmt[0] = wdwTrLmtOrig[0];                      // Reset Lower Row Limit
                    wdwTrLmt[1] = wdwTrLmtOrig[1];                      // Reset Lower Column Limit
                    wdwTrLmt[2] = wdwTrLmtOrig[2];                      // Reset Upper Row Limit
                    wdwTrLmt[3] = wdwTrLmtOrig[3];                      // Reset Upper Column Limit


                    // Print out Parameters
                    // ******************** //
                    std::cout << SCL        // Current Scaling Parameter
                              << ", "       //
                              << SKR        // Current Row (Y) Skew Parameter
                              << ", "       //
                              << SKC        // Current Col (X) Skew Parameter
                              << ", "       //
                              << ROT        // Current Rotation Parameter
                              << ", "       //
                              << mutInfo    // Current Mutual Information
                              << ", "       //
                              << normSize   // Current Normalization (Total Used Pixels)
                              << std::endl; //
                }

                // Print out Max Value
                // ******************** //
                std::cout << maxMutInfo
                          << std::endl
                          << std::endl;
            }
        }
    }


    return;  // Function Returns optimalParam by Reference
}
