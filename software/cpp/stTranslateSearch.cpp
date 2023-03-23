#include "stStitchingParams.h"

void stTranslateSearch( const cv::Mat& imEdgeA,
                        const cv::Mat& imEdgeB,
                        const int& numEdge,
                        int& normSize,
                        const int* wdwTrLmt,
                        int& TRR,
                        int& TRC,
                        cv::Point& maxLapMutInfoXY,
                        double& maxLapMutInfo )
{
    // **************************************** //
    assert( imEdgeA.cols == imEdgeB.cols );     // Force Images to be the Same Size
    assert( imEdgeA.rows == imEdgeB.rows );     // or fail if they are not.

    int imRows = imEdgeA.rows;                  // Store Rows
    int imCols = imEdgeA.cols;                  // Store Cols

    int szMapRow = 2*imRows - 1;                // Set Mutual Info Map Sizes.
    int szMapCol = 2*imCols - 1;                // 1 is Subtracted, otherwise it will duplicate the center overlap.

    uchar* ptrEdgeA = imEdgeA.data;             // Pointer to Edge Image Data
    uchar* ptrEdgeB = imEdgeB.data;             // Pointer to Edge Image Data


    // **************************************** //
    int ovpMaxR = 0;                            // Storage variables for the extremes of
    int ovpMaxC = 0;                            // the overlap windows in the translation search.

    double valMutInfo = 0;                      // Storage for Mutual Info.
    double valEntA = 0;                         // Storage for Entropy
    double valEntB = 0;                         // Storage for Entropy

    int pixNorm = 0;                            // Histogram Normalization Factor
    int maxNormAB = 0;                          //

    int ovpPixA = (imCols*imRows)-1;            // Starting Point of Edge Image A Overlap Rectangle Indices
    int ovpPixB = 0;                            // Starting Point of Edge Image B Overlap Rectangle Indices
    int ovpValA = 0;                            // Histogram Bin from Overlap Region
    int ovpValB = 0;                            // Histogram Bin from Overlap Region
    int idxA = 0;
    int idxB = 0;

    // numEdge is 1 less than the number of Levels in imEdgeA and imEdgeB
    // ************************************************ //
    double maxEntA = 0;                                 // Maximum Entropy Value
    double maxEntB = 0;                                 // Maximum Entropy Value
    std::vector<int> histA(numEdge+1);                  // Marginal Histogram, include 0 value
    std::vector<int> histB(numEdge+1);                  // Marginal Histogram, include 0 value
    std::vector<int> histAB((numEdge+1)*(numEdge+1));   // Joint Histogram
    int* ptrHistA = &histA[0];                          // Marginal Histogram, include 0 value
    int* ptrHistB = &histB[0];                          // Marginal Histogram, include 0 value
    int* ptrHistAB = &histAB[0];                        // Joint Histogram


    // Mutual Information Map
    // ************************************************ //
    cv::Mat mapMutInfo = cv::Mat::zeros( szMapRow,      // Initialize Map to Zeros
                                         szMapCol,      //
                                         CV_64F );      //

    double* ptrMapMutInfo = (double *)mapMutInfo.data;  // Pointer to modify data

    cv::Mat mapWeight = cv::Mat::zeros( szMapRow,       // Initialize Map to Zeros
                                        szMapCol,       //
                                        CV_64F );       //

    double* ptrMapWeight = (double *)mapWeight.data;    // Pointer to modify data


    // Filtered Mutual Information Map
    // ************************************************************ //
    cv::Mat mapLapMutInfo = cv::Mat::zeros( szMapRow,               // Initialize Map to Zeros
                                            szMapCol,               //
                                            CV_64F );               //

    double* ptrMapLapMutInfo = (double *)mapLapMutInfo.data;        // Pointer to modify data

    cv::Point center(-1,-1);                                        // Convolution Mask Pivot Point (-1,-1) == (Center of Mask)
    double alpha = 0.90;                                            // Laplacian Filter Factor
    double arrayLaplacian[9] = { (4./(alpha+1.))*(alpha/4.),        // Laplacian Filter Mask
                                 (4./(alpha+1.))*((1.-alpha)/4.),   //
                                 (4./(alpha+1.))*(alpha/4.),        //
                                 (4./(alpha+1.))*((1.-alpha)/4.),   //
                                 (4./(alpha+1.))*-1.,               //
                                 (4./(alpha+1.))*((1.-alpha)/4.),   //
                                 (4./(alpha+1.))*(alpha/4.),        //
                                 (4./(alpha+1.))*((1.-alpha)/4.),   //
                                 (4./(alpha+1.))*(alpha/4.) };      //

    cv::Mat maskLaplacian( 3, 3, CV_64F, arrayLaplacian );          // Store Array of Doubles in cv::Mat of doubles


    // ************************************************************************ //
    for( int i = wdwTrLmt[0]; i < wdwTrLmt[2]; i++ )                            // Loop through image Rows
    {
        for( int j = wdwTrLmt[1]; j < wdwTrLmt[3]; j++ )                        // Loop through image Cols
        {
            pixNorm = 0;                                                        // Clear Joint Normalization Factor

            memset( ptrHistA, 0, sizeof(int) * (numEdge+1) );                   // Instantiate and Clear Marginal Histogram Memory
            memset( ptrHistB, 0, sizeof(int) * (numEdge+1) );                   // Instantiate and Clear Marginal Histogram Memory
            memset( ptrHistAB, 0, sizeof(int) * (numEdge+1) * (numEdge+1) );    // Instantiate and Clear Joint Histogram Memory

            // ******************************************************************** //
            ovpPixA = MAX( (imRows-1) - i, 0 )*imCols + MAX( (imCols-1) - j, 0);    // Overlap Region Start (Bottom Right)
            ovpPixB = MAX( i - (imRows-1), 0 )*imCols + MAX( j - (imCols-1), 0);    // Overlap Region Start (Top Left)

            ovpMaxR = ( (imRows-1) - abs(i-(imRows-1)) );                           // Constrain Overlap Region
            ovpMaxC = ( (imCols-1) - abs(j-(imCols-1)) );                           // Constrain Overlap Region

            for( int ovpR = 0; ovpR <= ovpMaxR; ovpR++ )                            // Loop through Overlap Rectangle Rows
            {
                for( int ovpC = 0; ovpC <= ovpMaxC; ovpC++ )                        // Loop through Overlap Rectangle Cols
                {
                    idxA = ovpPixA + (ovpC + ovpR*imCols);                          // Edge Map Index within Overlap
                    idxB = ovpPixB + (ovpC + ovpR*imCols);                          // Edge Map Index within Overlap

                    ovpValA = ptrEdgeA[idxA];                                       // Edge Value is Histogram Bin (+1)
                    ovpValB = ptrEdgeB[idxB];                                       // Edge Value is Histogram Bin (+1)

                    if( (ovpValA*ovpValB) != 0 )                                    // Ignore 0 values
                    {
                        // -1 so that the histogram starts at 0.
                        // so hist[x] is the count of x+1.
                        ptrHistA[ ovpValA-1 ] += 1;                                 // Edge Value used as Histogram Location for Increment
                        ptrHistB[ ovpValB-1 ] += 1;                                 // Edge Value used as Histogram Location for Increment
                        ptrHistAB[ (ovpValB-1) + (ovpValA-1)*(numEdge+1) ] += 1;    // Edge Value used as Histogram Location for Increment
                        pixNorm += 1;                                               // Increment Normalization factor
                    }
                }
            }


            // Store maximum pixel count from overlap so that mutual information can be
            // weighted by the number of pixels used to generate it. Mut. Info. is a probabilistic
            // measure, and the lack of data increases the probability of a high mutual information
            // which results in false peaks in the search for the globabl maximum.
            // This weighting helps, but was not sufficient, and so that's why the laplacian
            // filter is there, so that we will find the optimal local maximum instead of a
            // global (false) maximum.
            // **************************************************************************** //
            maxNormAB = ( pixNorm > maxNormAB )                                             // Is Current Value greater than maximum?
                        ? pixNorm                                                           // Store current value if true
                        : maxNormAB;                                                        // Store maximum if false

            valMutInfo = 0;                                                                 // Storage for Mutual Info.
            valEntA = 0;                                                                    // Storage for Entropy
            valEntB = 0;                                                                    // Storage for Entropy

            stMutualInformation( &histA[0],                                                 // Overlap Marginal Histogram
                                 &histB[0],                                                 // Overlap Marginal Histogram
                                 &histAB[0],                                                // Overlap Joint Histogram
                                 pixNorm,                                                   // Pass Normalization Factor
                                 numEdge,                                                   // Pass Number of Edges
                                 valMutInfo,                                                // Overlap Mutual Info Value
                                 valEntA,                                                   // Entropy of Edge Image A Overlap
                                 valEntB );                                                 // Entropy of Edge Image B Overlap

            ptrMapMutInfo[ j + i*szMapCol ] = (2. * valMutInfo) / (valEntA + valEntB);  // Normalized Mutual Information
            ptrMapWeight[ j + i*szMapCol ] = pixNorm;                                   // Weighting value

        }
    }

    // **************************************** //
    mapWeight /= (double)maxNormAB;             // Normalize the weighting map

    mapMutInfo = mapMutInfo.mul( mapWeight );   // Multiply (Element-wise) Mutual Info map by weighting

    normSize = maxNormAB;                       // Store maximum normalization for return


    // See above comment for explanation of filter.
    // **************************************** //
    cv::filter2D( mapMutInfo,                   // Input Image
                  mapLapMutInfo,                // Output Image
                  CV_64F,                       // Output Type
                  maskLaplacian,                // Filter/Conv. Mask (cv::Mat)
                  center,                       // Pivot point for Mask (-1,-1) == Center
                  0,                            // Data Shift Value
                  cv::BORDER_REPLICATE );       // Border Type (Prevents Border Peaks)


    // ************************************************ //
    for( int i = 0; i < szMapRow*szMapCol; i++ )        // Step through entire map
    {
        // The laplacian filter produces negatives, just meaning
        // the direction of the change, so these are all set to
        // positive to make the peak search omni-directional.
        // (in the confines of the directionality of the filter)
        // ******************************************** //
        ptrMapLapMutInfo[i] = ptrMapLapMutInfo[i] < 0   // Is value negative?
                              ? -ptrMapLapMutInfo[i]    // Make value positive if true
                              : ptrMapLapMutInfo[i];    // Keep value if false

        // This enforces a minimum of 30% overlap
        // This is necessary, otherwise projective images
        // would not register. They have too many false peaks
        // when you have less than 30% pixel overlap
        // ******************************************** //
        ptrMapLapMutInfo[i] = ptrMapWeight[i] < 0.3     // Is weighting outside bounds?
                              ? 0                       // Set value to zero if true
                              : ptrMapLapMutInfo[i];    // Keep value if false
    }

    cv::minMaxLoc( mapLapMutInfo,                       // Map to search for extremes in
                   0,                                   // Minimum Value (Not Used)
                   &maxLapMutInfo,                      // Maximum Value
                   0,                                   // Minimum Location (Not Used)
                   &maxLapMutInfoXY );                  // Find max value's map location


    // END OF FILE
    // ************************************ //
    TRR = maxLapMutInfoXY.y - imRows + 1;   // Row Translation Parameter
    TRC = maxLapMutInfoXY.x - imCols + 1;   // Col Translation Parameter
    return;                                 // Return Translations by reference
} 
