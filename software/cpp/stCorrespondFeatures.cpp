#include "stStitchingParams.h"
#include <fstream>

void stCorrespondFeatures( const cv::Mat& imGradA,
						   const cv::Mat& imGradB,
						   const std::vector<cv::Point2f>& pxCornerA,
						   const std::vector<cv::Point2f>& pxCornerB,
						   cv::Mat& tformProjAB )
{
	// ************************************************ //
	int numCornersA = pxCornerA.size();					// Number of found corners in A
	int numCornersB = pxCornerB.size();					// Number of found corners in B

	int imRows = imGradA.rows;							// Number of image rows
	int imCols = imGradA.cols;							// Number of image columns

	std::vector<cv::Point2i> pxCornerAB( numCornersA );	// vector of point correspondences
	std::vector<double> pxMutInfoAB( numCornersA );		// Vector of mutual information values

	int szWindow = 4;									// Window square radius
	
	int maxR = szWindow;								// Maximum Row value in window
	int maxC = szWindow;								// Maximum Col value in window
	int minR = -szWindow;								// Minimum row value in window
	int minC = -szWindow;								// Minimum col value in window
	int whtWndw = (2*szWindow+1)*(2*szWindow+1);		// Search Window total size for weighting


	// With only a static window based mutual information
	// search, it seems necessary to base the resolution
	// of the data on the size of the data collected.
	// With a large number of edges the probability of
	// matching non-exact data sets goes way down. With
	// a smaller number you have the opposite effect
	// and disparaging sets of data appear identical.
	// Especially because mutual information is a
	// spacially independent calculation.
	// This factor of 5 has been working, but can be
	// modified given experimental results.
	// ************************************************ //
	int numEdges = (int)ROUND( whtWndw / 5. );			// Base the Gradient Quantization on the Search Window Size

	double distCorners = 40;							// Allowable world coordinates' distance between possible point pairs

	int validCrspndnc = 0;								// Storage for valid correspondences counter
	
	std::vector<int> histA(numEdges+1);					// Marginal Histogram, include 0 value
	std::vector<int> histB(numEdges+1);					// Marginal Histogram, include 0 value
	std::vector<int> histAB((numEdges+1)*(numEdges+1));	// Joint Histogram
	
	int* ptrHistA = &histA[0];							// Marginal Histogram, include 0 value
	int* ptrHistB = &histB[0];							// Marginal Histogram, include 0 value
	int* ptrHistAB = &histAB[0];						// Joint Histogram


	// ************************************************************ //
	cv::Mat imEdgeA = cv::Mat::zeros( imGradA.size(), CV_8U );		// Quantized Gradient image
	cv::Mat imEdgeB = cv::Mat::zeros( imGradB.size(), CV_8U );		// Quantized Gradient image

	stGradToEdge( imGradA,											// Quantize the gradient image
				  numEdges,											// The number of quantization levels
				  imEdgeA );										// The resultant "Edge" image

	stGradToEdge( imGradB,											// Quantize the gradient image
				  numEdges,											// The number of quantization levels
				  imEdgeB );										// The resultant "Edge" image

	
	// Loop through Corner Pairs to choose correspondences based on Mut Info
	// ******************************************************************** //
	for( int iCrnrA = 0; iCrnrA < numCornersA; iCrnrA++ )					// Loop through A's Corners
	{
		double maxMutInfoAB = 0.;											// Maximum Mutual Information Value for Matching

		for( int iCrnrB = 0; iCrnrB < numCornersB; iCrnrB++ )				// Loop through B's Corners
		{
			cv::Point2f ptA = pxCornerA[iCrnrA];							// Current Corner from A
			cv::Point2f ptB = pxCornerB[iCrnrB];							// Current Corner from B

			if( norm(ptB-ptA) > distCorners )								// If P2 Norm Distance between corners
			{																//    is outside the acceptable range
				continue;													//	  then go to next pair of corners.
			}

			int pixNorm = 0;												// Clear Normalization Factor
            
            memset( ptrHistA, 0, sizeof(int) * (numEdges+1) );					// Clear Marginal Histogram
			memset( ptrHistB, 0, sizeof(int) * (numEdges+1) );					// Clear Marginal Histogram
			memset( ptrHistAB, 0, sizeof(int) * (numEdges+1) * (numEdges+1) );		// Clear Joint Histogram

			
			// Loops for Windows Centered around Corner Pixels
			// ************************************************************ //
			for( int i = minR; i <= maxR; i++ )								// Loop from -szWindow to szWindow
			{														
				for( int j = minC; j <= maxC; j++ )							// Loop from -szWindow to szWindow
				{													
					int idxA = (int)( (ptA.x+j)+(ptA.y+i)*imGradA.cols );	// Index for Image A based on Corner Locations
					int idxB = (int)( (ptB.x+j)+(ptB.y+i)*imGradB.cols );	// Index for Image B based on Corner Locations

					if( (idxA < 0) ||										// Is the Index Less than Zero?
						(idxB < 0) ||										// Is the Index Less than Zero?
						(idxA > imRows*imCols-1) ||							// Has the Index Left the Image?
						(idxB > imRows*imCols-1) )							// Has the Index Left the Image?
					{
						continue;											// If any are true, go to next indices.
					}

					int GAxy = (int)(imEdgeA.data[idxA]);					// Corner Value from Edge Map
					int GBxy = (int)(imEdgeB.data[idxB]);					// Corner Value from Edge Map

					if( (GAxy*GBxy) != 0 )									// Ignore 0 values from Edge Map
					{
						ptrHistA[ GAxy-1 ] += 1;							// Marginal Histogram Increments
                        ptrHistB[ GBxy-1 ] += 1;							// Marginal Histogram Increments
                        ptrHistAB[(GBxy-1) + (GAxy-1)*(numEdges+1)] += 1;	// Joint Histogram Increments
                        pixNorm += 1;										// Normalization Increments
					}
				}
			}

			
			// Determine Mutual Info and check against minimum
			// ************************************************ //
			double valMutInfo = 0;								// Storage for Mutual Info.
			double valEntA = 0;									// Storage for Entropy
			double valEntB = 0;									// Storage for Entropy
			
			
			stMutualInformation( &histA[0],						// Marginal Histogram
								 &histB[0],						// Marginal Histogram
								 &histAB[0],					// Joint Histogram
								 pixNorm,						// Pass Normalization for Joint Hist.
								 numEdges,						// Pass Number of Edges
								 valMutInfo,					// Mutual Info Value
								 valEntA,						// Entropy of Edge Window A
								 valEntB );						// Entropy of Edge Window B

			valMutInfo = (2.*valMutInfo) / (valEntA + valEntB);	// Normalize Mutual information
			valMutInfo *= (pixNorm/whtWndw);					// Weight mutual information by valid Window
																// If less pixels were used, the result is less reliable (probabilistically)

			if( valMutInfo > maxMutInfoAB )						// Store only when current value exceeds maximum
			{	
				pxCornerAB[validCrspndnc].y = iCrnrB;			// Store index for Point in B for correspondence
				pxMutInfoAB[validCrspndnc] = valMutInfo;		// Store Mutual Information Value
			}
			
			maxMutInfoAB = ( valMutInfo > maxMutInfoAB )		// Update maximum mutual information value
							? valMutInfo						// Store current if greater
							: maxMutInfoAB;						// Store original if greater
		}
		
		pxCornerAB[validCrspndnc].x = iCrnrA;					// Store index for Point in A for correspondence
		
		validCrspndnc++;										// Valid point correspondence made, increment counter
	}


	// Rearrange points for sorted point correspondences
	// Matching pairs' indices were stored, and now they're used to create
	// 1-by-1-by-2 arrays, where location (a,1) is X and location (a,2) is
	// Y, and the same location in Array A and Array B relate the
	// corresponding points. This is a list with a certain number of outliers
	// that the RANSAC search should eliminate. Obviously the more outliers
	// the harder this becomes, especially being an automatic algorithm.
	// **************************************************************** //
	cv::Mat pxSortA( pxCornerAB.size(), 1, CV_32FC2 );					// Storage for sorted array of points
	cv::Mat pxSortB( pxCornerAB.size(), 1, CV_32FC2 );					// Storage for sorted array of points

	float* ptrPxSortA = (float*)pxSortA.data;							// Pointer to point location data
	float* ptrPxSortB = (float*)pxSortB.data;							// Pointer to point location data
	
	for( int i = 0; i < validCrspndnc; i++ )							// Loop through total number of valid pairings
	{
		ptrPxSortA[i+(i%2)] = (float)(pxCornerA[ pxCornerAB[i].x ].x);	// Column Location for A
		ptrPxSortA[i+1] = (float)(pxCornerA[ pxCornerAB[i].x ].y);		// Row Location for A
		
		ptrPxSortB[i+(i%2)] = (float)(pxCornerB[ pxCornerAB[i].y ].x);	// Column Location for B
		ptrPxSortB[i+1] = (float)(pxCornerB[ pxCornerAB[i].y ].y);		// Row Location for B
	}
	
	
	// Iterative Determination of Homography
	// ******************************************** //
	tformProjAB = cv::findHomography( pxSortA,		// Sorted Array of Corresponding Points in A
									  pxSortB,		// Sorted Array of Corresponding Points in B
									  CV_RANSAC,	// Perform RANSAC Operation
									  1 );			// RANSAC Outlier Factor, Results should be within 1 pixel accuracy

	// ******************************************** //
	//
	// TODO: Verify that this tranform makes sense
	// Currently there is no constraint on resultant
	// image size or coherence.
	//
	// ******************************************** //

	// END OF FILE
	// ************ //
	return;			// Function Returns tformProjAB by reference
}
