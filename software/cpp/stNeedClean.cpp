#include "stStitchingParams.h"

int stNeedClean( const cv::Mat& imColorA,
                 const cv::Mat& imColorB )
{
    // ************************************ //
    int needCleaning;                       // Decision Variable

    cv::Scalar avgImColorA;                 // 3 Valued "Vector" of Color Channel Means
    cv::Scalar avgImColorB;                 // 3 Valued "Vector" of Color Channel Means


    // This is a simple check to see if the results are visually
    // different, which can cause the gradients to be different and
    // the registration to fail. A more complex method could be
    // put in place, but given research constraints an automatic
    // method was not known at the time, without much more
    // extensive research. Currently color difference is the only
    // measure, even though the images are also sharpened if
    // "needCleaning" is non-zero. Again, an automatic method for
    // accurately determining image "sharpenness" was not known
    // without more extensive research. And sharpening of low entropy
    // (repeated texture) images can actually sway the registration
    // to fail. For example, tiled floors can match in arbitrary
    // orientations, especially if sharpened to make their pattern
    // have a higher gradient result.
    // ************************************************************ //
    avgImColorA = cv::mean( imColorA );                             // Get Channel Averages from First Image
    avgImColorB = cv::mean( imColorB );                             // Get Channel Averages from Second Image

    double dfcImColorB = abs( avgImColorA[0] - avgImColorB[0] );    // Blue Channel Average Absolute Difference
    double dfcImColorG = abs( avgImColorA[1] - avgImColorB[1] );    // Green Channel Average Absolute Difference
    double dfcImColorR = abs( avgImColorA[2] - avgImColorB[2] );    // Red Channel Average Absolute Difference

    // The Human Visual System is more sensitive to Green Changes.
    needCleaning = ( ( dfcImColorB > 10 ) ||                        // Blue Color Difference > 10 Values ?
                     ( dfcImColorG > 5 ) ||                         // Green Color Difference > 10 Values ?
                     ( dfcImColorR > 10 ) )                         // Red Color Difference > 10 Values ?
                   ? 1                                              // If so, then images need Adjustment
                   : 0;                                             // If not, then images don't need Adjustment

    // END OF FILE
    // ************************ //
    return needCleaning;        // Return Decision Variable
}
