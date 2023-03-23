#include "stStitchingParams.h"

void stGradPyramid(
      const int & numPymd
    , std::vector<cv::Mat> & imPymd
)
{
    // ???
    // ********************************************************************** //
    cv::Mat maskGauss(5, 5, CV_64F);                    // Instantiate Filter Mask
    double * ptrMaskGauss = (double *)maskGauss.data;   // Pointer to mask data

    double G0 = 0.25 - (0.375 / 2.0);               // 1D Filter Values
    double G1 = 0.25;                               //
    double G2 = 0.375;                              //
                                                    // Create 2D filter from Matrix Multiplication of 1D filter
                                                    // and its transpose:
    // Loop Unwrapping                              // **************************************************** //
    ptrMaskGauss[0]  = G0 * G0;                     //      y                      x                        //
    ptrMaskGauss[1]  = G0 * G1;                     //      |       _              |        _               //
    ptrMaskGauss[2]  = G0 * G2;                     //      |     /   \            |      /   \             //
    ptrMaskGauss[3]  = G0 * G1;                     //      |    /     \       (x) |     /     \            //
    ptrMaskGauss[4]  = G0 * G0;                     //      |  _/       \_         |   _/       \_          //
    ptrMaskGauss[5]  = G1 * G0;                     //      |_ _ _ _ _ _ _ _ _ x   |_ _ _ _ _ _ _ _ _ y     //
    ptrMaskGauss[6]  = G1 * G1;                     //                                                      //
    ptrMaskGauss[7]  = G1 * G2;                     //                          =                           //
    ptrMaskGauss[8]  = G1 * G1;                     //              z                                       //
    ptrMaskGauss[9]  = G1 * G0;                     //              |                                       //
    ptrMaskGauss[10] = G2 * G0;                     //              |                                       //
    ptrMaskGauss[11] = G2 * G1;                     //              |                                       //
    ptrMaskGauss[12] = G2 * G2;                     //              |      /@\                              //
    ptrMaskGauss[13] = G2 * G1;                     //              |     /   \                             //
    ptrMaskGauss[14] = G2 * G0;                     //              |_ _ /     \_ _ _ x                     //
    ptrMaskGauss[15] = G1 * G0;                     //             /    /       \                           //
    ptrMaskGauss[16] = G1 * G1;                     //            /    /         \                          //
    ptrMaskGauss[17] = G1 * G2;                     //          _/_ _ (@@@@@@@@@@@) _ _ _                   //
    ptrMaskGauss[18] = G1 * G1;                     //          /                                           //
    ptrMaskGauss[19] = G1 * G0;                     //         y                                            //
    ptrMaskGauss[20] = G0 * G0;                     // **************************************************** //
    ptrMaskGauss[21] = G0 * G1;                     //
    ptrMaskGauss[22] = G0 * G2;                     //
    ptrMaskGauss[23] = G0 * G1;                     //
    ptrMaskGauss[24] = G0 * G0;                     //
    // ********************************************************************** //


    // ???
    // ********************************************************************** //
    // OpenCV has a pyramid function, by just repeating it's downsampling
    // function, but the gaussian kernel they used was not in-line with MATLAB
    // and the research behind MATLAB's pyramid function, so to avoid disparate
    // results, the function was rewritten here as a series of filtering and
    // decimation
    // ********************************************************************** //
    cv::Point maskCtr(-1, -1);                       // Set filter mask pivot to mask center point (OpenCV notation)

    for(int p = 0; p < numPymd; ++p)              // Step through pyramid levels
    {
        // ???
        // ****************************************************************** //
        int origRows = imPymd[p].rows;              // Store current pyramid level total rows
        int origCols = imPymd[p].cols;              // Store current pyramid level total cols

        int nextRows = (int)ceil(origRows/2.);      // Store next pyramid level total rows
        int nextCols = (int)ceil(origCols/2.);      // Store next pyramid level total cols

        imPymd[p + 1] = cv::Mat::zeros(
              nextRows      // Instantiate next pyramid level with array of zeros
            , nextCols      //
            , CV_32F        //
        );

        cv::Mat imBlur = cv::Mat::zeros(
              origRows  // Instantiate filtered image with array of zeros
            , origCols  //
            , CV_32F    //
        );

        cv::filter2D(
              imPymd[p]                 // Input Current Level to be filtered
            , imBlur                    // Result of Filtering
            , CV_32F                    // Result Data Type
            , maskGauss                 // Filter Kernel/Mask
            , maskCtr                   // Mask Pivot Point (-1,-1) == (Mask Center)
            , 0                         // Shift factor
            , cv::BORDER_REPLICATE      // Border Interpolation, Replicate avoids border peaks
        );
        // ****************************************************************** //

        // ???
        // ****************************************************************** //
        float *imNextBlur = (float*)(imPymd[p+1].data);                     // Pointer to next level's data
        float *imOrigBlur = (float*)(imBlur.data);                          // Pointer to current level's filtered data

        int i = 0;                                                          // Initialize Next Level's Row Index

        for( int r = 0; r < origRows; r += 2 )                              // Loop through Current Level's Rows (every other)
        {
            int j = 0;                                                      // Intialize Next Level's Col Index

            for( int c = 0; c < origCols; c += 2 )                          // Loop through Current Level's Cols (every other)
            {
                float nextVal = (float)ROUND( imOrigBlur[(r*origCols)+c] ); // Round value from current level
                imNextBlur[(i * nextCols) + j] = nextVal;                   // Store rounded value in next level
                ++j;                                                        // Increment Next Level's Col Index
            }

            ++i;                                                            // Increment Next Level's Row Index
        }
        // ****************************************************************** //
    }
    // ********************************************************************** //

    return;
}
