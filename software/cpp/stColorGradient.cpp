#include "stStitchingParams.h"

void stColorGradient( const cv::Mat& imColor,
                      cv::Mat& imGradient )
{
    // ************************************************************ //
    // Mathematical Formula of Maximum Eigenvalue:                  //
    //                                                              //
    // EIG = (0.5*(P+Q+((P+Q)^2-(4*(P.mul(Q)-(T^2))))^0.5))^0.5;    //
    // ************************************************************ //

    // ******************************************** //
    int imRows = imColor.rows;                      // Input Image No. of Rows
    int imCols = imColor.cols;                      // Input Image No. of Columns
    cv::Mat imCalc( imRows, imCols, CV_32FC3 );     // Create new Floating Point version of Input Image
    imColor.convertTo( imCalc, CV_32FC3, 1, 0 );    // Upconvert from Unsigned 8-bit to Double


    // ******************************************** //
    cv::Mat cDiff( imRows, imCols, CV_32FC3 );      // Row Partial Discrete Derivatives
    cv::Mat rDiff( imRows, imCols, CV_32FC3 );      // Col Partial Discrete Derivatives
    cv::Mat xDiff( imRows, imCols, CV_32FC3 );      // Multiplied Partials
    cv::Mat cSumDiff( imRows, imCols, CV_32FC1 );   // Sum of Row Partials (Q)
    cv::Mat rSumDiff( imRows, imCols, CV_32FC1 );   // Sum of Col Partials (P)
    cv::Mat xSumDiff( imRows, imCols, CV_32FC1 );   // Sum of Multiplied Partials (T)


    // ******************************************************************** //
    cv::Scharr( imCalc, cDiff, CV_32F, 1, 0, 1, 0, cv::BORDER_REPLICATE );  // Column Derivative
    cv::Scharr( imCalc, rDiff, CV_32F, 0, 1, 1, 0, cv::BORDER_REPLICATE );  // Row Derivative


    // ************************ //
    xDiff = cDiff.mul(rDiff);   // Multiplied Partials
    cv::pow(cDiff, 2, cDiff);   // Squares of Partials in Row Direction
    cv::pow(rDiff, 2, rDiff);   // Squares of Partials in Col Direction


    // **************************************************** //
    cv::Mat ptrImCn[3];                                     // Array List of Temporary Storage
    ptrImCn[0] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[1] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[2] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    cv::split( cDiff, ptrImCn );                            // Parse Channels of Row Partial
    cSumDiff = ptrImCn[0] + ptrImCn[1] + ptrImCn[2];        // Sum Row Partial's Channels


    ptrImCn[0] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[1] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[2] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    cv::split( rDiff, ptrImCn );                            // Parse Channels of Col Partial
    rSumDiff = ptrImCn[0] + ptrImCn[1] + ptrImCn[2];        // Sum Col Partial's Channels


    ptrImCn[0] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[1] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    ptrImCn[2] = cv::Mat::zeros( imRows, imCols, CV_32F );  //
    cv::split( xDiff, ptrImCn );                            // Parse Channels of Multiplied Partials
    xSumDiff = ptrImCn[0] + ptrImCn[1] + ptrImCn[2];        // Sum Multiplied Partials' Channels


    // **************************************************** //
    cv::Mat TEMP1( imRows, imCols, CV_32F );                // Temporary Variable 1
    TEMP1 = cv::Mat::zeros(imRows, imCols, CV_32F);         // Instantiate to Zeros
    cv::Mat TEMP2( imRows, imCols, CV_32F );                // Temporary Variable 2
    TEMP2 = cv::Mat::zeros(imRows, imCols, CV_32F);         // Instatiate to Zeros

    cv::pow( xSumDiff, 2, xSumDiff );                       // T^2
    TEMP1 = cSumDiff.mul(rSumDiff);                         // P*Q
    TEMP1 -= xSumDiff;                                      // (P*Q - T^2)
    TEMP1 *= 4;                                             // 4*(P*Q - T^2)
    cv::add( cSumDiff, rSumDiff, TEMP2 );                   // (P + Q)
    cv::pow( TEMP2, 2, TEMP2 );                             // (P + Q)^2
    imGradient = TEMP2 - TEMP1;                             // (P+Q)^2 - 4*(P*Q - T^2)
    cv::sqrt(imGradient, imGradient);                       // sqrt((P+Q)^2 - 4*(P*Q - T^2))
    imGradient += cSumDiff;                                 // Q + sqrt((P+Q)^2 - 4*(P*Q - T^2)
    imGradient += rSumDiff;                                 // P + Q + sqrt((P+Q)^2 - 4*(P*Q - T^2)
    imGradient *= 0.5;                                      // 0.5*(P + Q + sqrt((P+Q)^2 - 4*(P*Q - T^2)))
    cv::sqrt(imGradient, imGradient);                       // sqrt(0.5*( P + Q + sqrt( (P+Q)^2 - 4*(P*Q - T^2) ) ))


    // **************************************** //
    float* ptrGrad = (float*)imGradient.data;   // Float data Pointer
    double minGrad, maxGrad;                    // Maximum of Gradient Map Storage

    cv::minMaxLoc( imGradient,                  // Array to search for extremes in
                   &minGrad,                    // Minimum Value
                   &maxGrad,                    // Maximum Value
                   0,                           // Minimum Location (Not Used)
                   0 );                         // Minimum Location (Not Used)


    // Normalize the Gradient Map so that it's values can be in the range of [1, MAX]
    // with MAX currently set to 10001, as an arbitrary scale to preserve the floating
    // point information. When scalled to integers in [1,255], the information was
    // lost when the gradient map was decimated in the multiresolution search.
    // The current range performs well, but could be modified. However, 0 is reserved
    // as "no data" and so is necessary to add 1 to keep the map from being 0.
    // **************************************************************************** //
    for( int i = 0; i < imRows*imCols; i++ )                                        // Loop through whole image
    {
        // Normalize Gradient Array between [1, 10001] floating point values
        ptrGrad[i] = (float)(10000. * ((double)(ptrGrad[i]) - minGrad)/(maxGrad-minGrad) ) + 1.f;
    }

    // END OF FILE
    // ************ //
    return;         //
}
