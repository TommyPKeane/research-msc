#include "stStitchingParams.h"

void stGradToEdge( const cv::Mat& imGrad,	// INPUT	Image Gradient
				   const int& numEdge,		// INPUT	Number of Quantization Levels
				   cv::Mat& imEdge )		// RESULT	Image Edge Map
{
	// ********************************************	//
	int imRows = imGrad.rows;						// Total Rows
	int imCols = imGrad.cols;						// Total Columns
	
	int histGrad[256];								// Gradient Histogram
	memset( histGrad, 0, sizeof(int)*256 );			// Initialize 256 Integer Values in Memory
	
	int histSum[256];								// Gradient Cumulative Histogram
	memset( histSum, 0, sizeof(int)*256 );			// Initialize 256 Integer Values in Memory
	
	std::vector<int> bounds(numEdge+2);				// Vector of histogram bin bounds for Quantization
													// numEdge+1 total Edges means numEdge+2 required bounds
	
	uchar* ptrEdge = imEdge.data;					// Pointer to Edge image data
	uchar* ptrGrad = imGrad.data;					// Pointer to Gradient image data					
	
	
	// ****************************************	//
	for( int i = 0; i < imCols*imRows; i++ )	// Loop through full image
	{
		int idxHist = ptrGrad[i];				// Index is Value of Gradient

		histGrad[idxHist] += 1;					// Increment histogram at index
	}


	// ******************************************** //
	for( int i = 1; i < 256; i++ )					// Loop through histogram values (start at 1, to ignore zeros)
	{
		histSum[i] = histSum[i-1] + histGrad[i];	// Cumulative Histogram Calculation												
	}

	int maxGrad = (int)ROUND(0.9*histSum[255]);		// Upper bound is at 90% (top 10% gets label numEdges)
	int minGrad = (int)ROUND(0.15*histSum[255]);	// Minimum bound is at 15% (bottom 15% gets label 1)
	

	// ******************************************** //
	int uB = 1;										// Upper Bound set to Minimum
	int histValue = histSum[0];						// Initialize Search Variable
	
	// Find where the 90% mark is
	while( (histValue<maxGrad) && (uB<=255) )		// Check for last Gradient value
	{
		histValue = histSum[++uB];					// Gradient distribution
	}

	uB--;											// Decrement Upper Bound to correct for extra increment

	int lB = uB;									// Restrict Lower Bound to Upper Bound
	histValue = histSum[uB];						// Reinitialize Search Variable
	
	// Find where the 15% mark is
	while( (histValue>minGrad) && (lB>=0) )			// Check for first Gradient value
	{
		histValue = histSum[--lB];					// Gradient distribution
	}

	lB++;											// Increment Lower Bound to correct for extra decrement

	
	// Calculate Histogram Bin Bounds based on numEdges
	// numEdge is 1 less than the resultant number of Quantization
	// levels to be found. i.e. EdgeMap contains values from {1,2,...,Q}
	// If numEdge is 0, no good. if numEdge is 2, Q is 3. if numEdge
	// is 1, Q is 2 (auto split at 128), and so on.
	// ******************************************************** //
	if( numEdge <= 0 )
	{
		MessageBox( NULL, "numEdge must be > 0", "", MB_OK );
		exit(1);
	}

	
	bounds[0] = 0;															// Initialize Bounds to 8-bit Range
	bounds[numEdge+1] = 255;												// 0 >___________> (???) >__________> 255

	if( numEdge >= 2 )														// Valid Range
	{
		bounds[1] = lB;														// These set the basic range (numEdge == 2) to:
		bounds[numEdge] = uB;												// 0 >______> LowerBound >________> UpperBound >_______> 255
	
		for( int j = 2; j < numEdge; j++ )									// Loop through mid region
		{
			bounds[j] = bounds[j-1] + (int)ROUND( 1.*(uB-lB)/(numEdge-1) );	// Split mid region into numEdge-1 equal bins
		}
	}
	else																	// Special Case: numEdge == 1
	{
		bounds[1] = 128;													// 0 >______> 128 >______> 255
	}
	
		
	// Avoid setting edge map to 0, because 0 means no data in algorithm
	// **************************************************************** //
	for( int i = 0; i < imRows*imCols; i++ )							// Loop through entire image
	{
		for( int j = 0; j < numEdge+1; j++ )							// Loop through bounds
		{
			if( (ptrGrad[i]>bounds[j]) && (ptrGrad[i]<=bounds[j+1]) )	// Change value if value is above lower bin bound and below or equal to upper bin bound
			{
				ptrEdge[i] = (uchar)(j+1);								// Edge Map gets appropriate value in [1,numEdge] range
			}
		}
	}


	// END OF FILE
	// ************ //
	return;			//
}
