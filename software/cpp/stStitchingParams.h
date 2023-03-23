//
// Main include for all function files.
// Calls stHomography for global functions and includes.
// All function prototypes go here.
//
 
#include "stHomography.h"

// stAFFINEPARAMSEARCH
// Transform Search that returns a 3x3 affine
// transformation Array
// **************************************** // VOID RETURN
void stAffineParamSearch( const double*,    // INPUT    Array of 12 Bounds on Transform, (lower, upper, step) 4 times
                          const cv::Mat&,   // INPUT    Gradient Image A
                          const cv::Mat&,   // INPUT    Gradient Image B
                          const int&,       // INPUT    Number of Edges for Quantization
                          cv::Mat&,         // RETURN   Optimal Affine Transform
                          double*,          // RETURN   Array of Optimal Transform Parameters (6)
                          int*,             // RETURN   Window Limits for Translation/MI Search (4) [LR, LC, UR, UC]
                          cv::Point&,       // RETURN   Location of Maximum Mut. Info. Value
                          double& );        // RETURN   Value of Maximum, Normalized, Filtered Mut. Info.


// stCOLORGRADIENT
// Finds Vector Field Gradient (RGB
// Color Gradient) and returns 8-bit
// quantized array
// ************************************ // VOID RETURN
void stColorGradient( const cv::Mat&,   // INPUT    Color Image (8-bit, 3 Chn.)
                      cv::Mat& );       // RETURN   Gradient Map (64-bit, 1 Chn.)



// stCORRESPONDFEATURES
// Uses vectors of corner points and performs mutual information
// window match to return a projective homography 3x3 array
// ************************************************************ // VOID RETURN
void stCorrespondFeatures( const cv::Mat&,                      // INPUT    Gradient Image Map A (64-bit, 1 Chn)
                           const cv::Mat&,                      // INPUT    Gradient Image Map B (64-bit, 1 Chn)
                           const std::vector<cv::Point2f>&,     // INPUT    Vector of Feature Points in A
                           const std::vector<cv::Point2f>&,     // INPUT    Vector of Feature Points in B
                           cv::Mat& );                          // RETURN   Projective Homography (64-bit, 3x3, 1 Chn)



// stGRADPYRAMID
// Develops a gaussian pyramid for multi-
// resolution search optimization
// ******************************************** // VOID RETURN
void stGradPyramid( const int&,                 // INPUT    Number of Levels in Pyramid - 1
                    std::vector<cv::Mat>& );    // RETURN   Vector of Images, Only 1st Level filled with Gradient of Input Image



// stGRADTOEDGE
// Thresholds the  Vector Field
// Gradient (i.e. RGB Color Gradient)
// to create an Edge Map
// ******************************** // VOID RETURN
void stGradToEdge( const cv::Mat&,  // INPUT    Image Gradient
                   const int&,      // INPUT    Number of Quantization Levels
                   cv::Mat& );      // RETURN   Image Edge Map



// stIMAGEENHANCE
// Performs a laplacian-based sharpen on
// the images and attempts to correct any
// color difference
// ************************************ // VOID RETURN
void stImageEnhance( const cv::Mat&,    // INPUT    Original Color Image A (Blurred) (8-bit, 3 Chn)
                     const cv::Mat&,    // INPUT    Original Color Image B (Blurred) (8-bit, 3 Chn)
                     cv::Mat&,          // RETURN   Enhanced Image A (8-bit, 3 Chn)
                     cv::Mat& );        // RETURN   Enhanced Image B (8-bit, 3 Chn)



// stMUTUALINFORMATION
// Calculates mutual information based
// on marginals and joint histogram
// ************************************ // VOID RETURN
void stMutualInformation( const int*,   // INPUT    Marginal Histogram of A
                          const int*,   // INPUT    Marginal Histogram of B
                          const int*,   // INPUT    Joint Histogram of A and B
                          const int&,   // INPUT    Number of Pixels used in Histogram Calculation
                          const int&,   // INPUT    Number of Levels in Data - 1
                          double&,      // RETURN   Mutual Information Value
                          double&,      // RETURN   Entropy of A
                          double& );    // RETURN   Entropy of B


// stNEEDCLEAN
// Checks for color difference in
// images, determines if they need
// correction
// ******************************** //
int stNeedClean( const cv::Mat&,    // INPUT    Color Image A (8-bit, 3 Chn)
                 const cv::Mat& );  // INPUT    Color Image B (8-bit, 3 Chn)


// stOVERLAPFEATURES
// Taks two images and returns vectors of their corner
// feature points
// **************************************************** // VOID RETURN
void stOverlapFeatures( const cv::Mat&,                 // INPUT    Color Image Map A (8-bit, 3 Chn)
                        const cv::Mat&,                 // INPUT    Color Image Map B (8-bit, 3 Chn)
                        std::vector<cv::Point2f>&,      // RETURN   Empty Vector of Max. No. of Points Size
                        std::vector<cv::Point2f>& );    // RETURN   Empty Vector of Max. No. of Points Size


// stTFORMSIZE
// Creates transform cv::Mat array
// and determines output image size
// ******************************** // VOID RETURN
void stTformSize( const cv::Size&,  // INPUT    Current Image Size
                  cv::Size&,        // RETURN   Transformed Image Size
                  cv::Mat& );       // RETURN   Transform to Apply to Image


// stTRANSLATESEARCH
// Performs exhaustive search and returns
// translation parameters by reference
// **************************************** // VOID RETURN
void stTranslateSearch( const cv::Mat&,     // INPUT    Edge Image A (8-bit, 1 Chn, numEdges + 1 Levels)
                        const cv::Mat&,     // INPUT    Edge Image B (8-bit, 1 Chn, numEdges + 1 Levels)
                        const int&,         // INPUT    Number of Levels in Edge Image - 1
                        int&,               // RETURN   Maximum Number of Pixels Used in Mut. Info. Map Calc. (Overwritten)
                        const int*,         // INPUT    Search Limits {Row First, Col First, Row Last, Col Last}
                        int&,               // RETURN   Translation Parameter (Row)
                        int&,               // RETURN   Translation Parameter (Col)
                        cv::Point&,         // RETURN   Location of Maximum (Filtered) Mutual Information (in Map)
                        double& );          // RETURN   Maximum Value of (Filtered) Mutual Information
