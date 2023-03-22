#include "stStitchingParams.h"

void stOverlapFeatures( const cv::Mat& imColorA,
                        const cv::Mat& imColorB,
                        std::vector<cv::Point2f>& pxCornerA,
                        std::vector<cv::Point2f>& pxCornerB )
{
    // **************************************************************** //
    int imRows = imColorA.rows;                                         // Total Image Rows
    int imCols = imColorA.cols;                                         // Total Image Cols

    cv::Mat imGrayA = cv::Mat::zeros( imRows, imCols, CV_8U );          // Instantiate Gray Image A
    cv::Mat imGrayB = cv::Mat::zeros( imRows, imCols, CV_8U );          // Instantiate Gray Image B

    cv::cvtColor( imColorA, imGrayA, CV_BGR2GRAY, 1 );                  // Convert A from Color to Gray
    cv::cvtColor( imColorB, imGrayB, CV_BGR2GRAY, 1 );                  // Convert B from Color to Gray

    cv::Mat imOverlapMask = cv::Mat::zeros( imGrayA.size(), CV_8U );    // Instantiate Overlap Mask

    imOverlapMask = (imGrayA.mul( imGrayB ) > 0)*255;                   // Determine Overlap Mask by Logical of Multiplication


    // Results will be top N number of "features", with N limited by parameter 4,
    // and the length of the corner vectors. The corner Vector lengths being a
    // maximum constraint, and parameter 4 of "goodFeaturesToTrack" being a
    // quality constraint. Harris factor was increased to 0.06 to find sharper edges
    // since the quality constraint is negligible. Corner distance and non-maximal
    // suppresion kernal size are in competition. Corner distances is automatically
    // at least the width of the non-maximal supression kernal away.
    // This method is not perfectly affine invariant, and can be replaced if a better
    // automatic feature extraction can be researched. There is also no constraint to
    // force features to spread out along multiple planes in the images so that the
    // homography found is optimal. If an automatic method for forcing the optimal
    // homography or optimal feature location correspondences could be researched,
    // that should be implemented here and in stCorrespondFeatures.
    // **************************************** //
    cv::goodFeaturesToTrack( imGrayA,           // Input Image
                             pxCornerA,         // Output Vector of Corner Locations
                             pxCornerA.size(),  // Maximum Number of Corners to Return
                             DBL_EPSILON,       // Acceptable Corners Related to Maximum
                             4,                 // Minimum Dist. Between Corners
                             imOverlapMask,     // Image Mask
                             3,                 // Non-Maxima Suppression Block Size (3x3) (Local Max)
                             true,              // Use Harris Corners?
                             0.06 );            // Harris Corner Factor (usually 0.04, higher for sharper)

    cv::goodFeaturesToTrack( imGrayB,           // Input Image
                             pxCornerB,         // Output Vector of Corner Locations
                             pxCornerB.size(),  // Maximum Number of Corners to Return
                             DBL_EPSILON,       // Acceptable Corners Related to Maximum
                             4,                 // Minimum Dist. Between Corners
                             imOverlapMask,     // Image Mask
                             3,                 // Non-Maxima Suppression Block Size(3x3) (Local Max)
                             true,              // Use Harris Corners?
                             0.06 );            // Harris Corner Factor (usually 0.04, higher for sharper)


    // END OF FILE
    // *************//
    return;         // Function returns pxCornerA and pxCornerB by reference
}
