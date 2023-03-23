#include "stStitchingParams.h"

void stImageEnhance( const cv::Mat& imColorA,
                     const cv::Mat& imColorB,
                     cv::Mat& imEnhanceA,
                     cv::Mat& imEnhanceB )
{
    //
    // Currently Sharpens images by addition of laplacian, then
    // shifts intensities so images are in the range of each other.
    // The laplacian addition does sharpen the images but can mess
    // with the color a little bit. This is just a basic method to
    // attenuate color difference and image quality effects, so the
    // algorithm can remain automatic.
    //

    // ******************************** //
    int imRows = imColorA.rows;         // Total Image Rows
    int imCols = imColorA.cols;         // Total Image Cols
    int imChan = imColorA.channels();   // Total Image Channels



    // ************************************************ //
    cv::Mat imDblColorA( imRows, imCols, CV_64FC3 );    // Storage for 64-bit Versions
    cv::Mat imDblColorB( imRows, imCols, CV_64FC3 );    // Storage for 64-bit Version

    imColorA.convertTo( imDblColorA,                    // Convert Image to 64-bit for Filtering Operation
                        CV_64FC3,                       //
                        1,                              //
                        0 );                            //

    imColorB.convertTo( imDblColorB,                    // Convert Image to 64-bit for Filtering Operation
                        CV_64FC3,                       //
                        1,                              //
                        0 );                            //


    // **************************************************** //
    cv::Scalar avgImColorA;                                 // Array for Channel Averages
    cv::Scalar avgImColorB;                                 // Array for Channel Averages

    avgImColorA = cv::mean( imColorA );                     // Determine per channel mean of image A
    avgImColorB = cv::mean( imColorB );                     // Determine per channel mean of image B

    cv::Scalar dfcImColor = ( avgImColorA - avgImColorB );  // Store per channel difference (signed) [A - B]


    // **************************************************** //
    cv::Mat imLapColorA( imRows, imCols, CV_64FC3 );        // Storage Declaration
    cv::Mat imLapColorB( imRows, imCols, CV_64FC3 );        // Storage Declaration

    cv::Point center(-1,-1);                                // Convolution Mask Pivot Point
    double krnLaplacian[9] = { -1, -1, -1,                  // Filter/Convolution mask
                               -1,  8, -1,                  // Basic Laplacian Operation
                               -1, -1, -1 };                //

    cv::Mat maskLaplacian( 3, 3, CV_64F, krnLaplacian );    // Store Array of Doubles in cv::Mat of doubles

    cv::filter2D( imDblColorA,                              // Input Image
                  imLapColorA,                              // Filtered Image Result
                  CV_64F,                                   // Result Data Type
                  maskLaplacian,                            // Filter Mask
                  center,                                   // Pivot Point
                  0,                                        // Data Mask to only Filter Certain Region
                  cv::BORDER_REPLICATE );                   // Border Method (Replicate Avoids Border Peaks)

    cv::filter2D( imDblColorB,                              // Input Image
                  imLapColorB,                              // Filtered Image Result
                  CV_64F,                                   // Result Data Type
                  maskLaplacian,                            // Filter Mask
                  center,                                   // Pivot Point
                  0,                                        // Data Mask to only Filter Certain Region
                  cv::BORDER_REPLICATE );                   // Border Method (Replicate Avoids Border Peaks)



    // ************************************************************ //
    double minValueA = 0;                                           // Minimum Intensity Value Storage
    double maxValueA = 0;                                           // Maximum Intensity value Storage

    double minValueB = 0;                                           // Minimum Intensity Value Storage
    double maxValueB = 0;                                           // Maximum Intensity value Storage

    cv::Mat ptrImLapColorA[3];                                      // Array For Image Channels
    cv::Mat ptrImLapColorB[3];                                      // Array for Image Channels

    ptrImLapColorA[0] = cv::Mat::zeros( imRows, imCols, CV_64F );   // Create Storage for Channels and Split Images
    ptrImLapColorA[1] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImLapColorA[2] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    cv::split( imLapColorA, ptrImLapColorA );                       // " "

    ptrImLapColorB[0] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImLapColorB[1] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImLapColorB[2] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    cv::split( imLapColorB, ptrImLapColorB );                       // " "

    cv::Mat ptrImDblColorA[3];                                      // " "
    cv::Mat ptrImDblColorB[3];                                      // " "

    ptrImDblColorA[0] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImDblColorA[1] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImDblColorA[2] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    cv::split( imDblColorA, ptrImDblColorA );                       // " "

    ptrImDblColorB[0] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImDblColorB[1] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    ptrImDblColorB[2] = cv::Mat::zeros( imRows, imCols, CV_64F );   // " "
    cv::split( imDblColorB, ptrImDblColorB );                       // " "

    for( int i = 0; i < 3; i++ )                                    // Loop through image(s) channels
    {
        cv::minMaxLoc( ptrImLapColorA[i],                           // Input image to find extremes in
                       &minValueA,                                  // Minimum Value
                       &maxValueA,                                  // Maximum Value
                       0,                                           // Minimum Location (not used)
                       0 );                                         // Maximum Location (not used)

        cv::minMaxLoc( ptrImLapColorB[i],                           // Input image to find extremes in
                       &minValueB,                                  // Minimum Value
                       &maxValueB,                                  // Maximum Value
                       0,                                           // Minimum Location (not used)
                       0 );                                         // Maximum Location (not used)

        double* ptrImColorA = (double*)(ptrImLapColorA[i].data);    // Pointer to laplacian filtered image A data
        double* ptrImColorB = (double*)(ptrImLapColorB[i].data);    // Pointer to laplacian filtered image B data
        double* ptrImColorA2 = (double*)(ptrImDblColorA[i].data);   // Pointer to image A data
        double* ptrImColorB2 = (double*)(ptrImDblColorB[i].data);   // Pointer to image B data


        // ******************************************************************************************** //
        for( int j = 0; j < imRows*imCols; j++ )                                                        // Loop through entire image(s)
        {
            ptrImColorA[j] = ROUND( 255. * ( (ptrImColorA[j]-minValueA) / (maxValueA-minValueA) ) );    // Normalize laplacian to [0,255]
            ptrImColorB[j] = ROUND( 255. * ( (ptrImColorB[j]-minValueB) / (maxValueB-minValueB) ) );    // Normalize laplacian to [0,255]
            ptrImColorA2[j] += ptrImColorA[j];                                                          // Add laplacian to original
            ptrImColorB2[j] += ptrImColorB[j];                                                          // Add laplacian to original
            ptrImColorA2[j] -= ( dfcImColor[i] / 2. );                                                  // Subtract Half the difference from A
            ptrImColorB2[j] += ( dfcImColor[i] / 2. );                                                  // Add Half the difference to B
        }


        // Initial normalization was to constrain the laplacian result
        // but now that it's been added an the color shift has been made
        // the image is out of the 8-bit range and must be normalized again
        // before converting back to 8-bits.
        // ******************************** //
        cv::minMaxLoc( ptrImDblColorA[i],   // Input image to find extremes in
                       &minValueA,          // Minimum Value
                       &maxValueA,          // Maximum Value
                       0,                   // Minimum Location (not used)
                       0 );                 // Maximum Location (not used)

        cv::minMaxLoc( ptrImDblColorB[i],   // Input image to find extremes in
                       &minValueB,          // Minimum Value
                       &maxValueB,          // Maximum Value
                       0,                   // Minimum Location (not used)
                       0 );                 // Maximum Location (not used)


        // ******************************************************************************************** //
        double* ptrImColorA3 = (double*)(ptrImDblColorA[i].data);                                       // New pointer to image data
        double* ptrImColorB3 = (double*)(ptrImDblColorB[i].data);                                       // New pointer to image data

        for( int j = 0; j < imRows*imCols; j++ )                                                        // Loop through entire image(s)
        {
            ptrImColorA3[j] = ROUND( 255. * ( (ptrImColorA3[j]-minValueA) / (maxValueA-minValueA) ) );  // Normalize Color Corrected Image to
            ptrImColorB3[j] = ROUND( 255. * ( (ptrImColorB3[j]-minValueB) / (maxValueB-minValueB) ) );  // Normalize Color corrected image to [0,255]
        }
    }

    // ************************************************ //
    cv::merge( ptrImDblColorA, 3, imDblColorA );        // Merge channels back into image
    cv::merge( ptrImDblColorB, 3, imDblColorB );        // Merge channels back into image

    imDblColorA.convertTo( imEnhanceA, CV_8UC3, 1, 0 ); // Convert double image back to 8-bit
    imDblColorB.convertTo( imEnhanceB, CV_8UC3, 1, 0 ); // Convert double image back to 8-bit

    // END OF FILE
    // ************ //
    return;         //
}
