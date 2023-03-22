#include "stStitchingParams.h"

int TestColorGradient();
int TestGradientThreshold();
int TestEdgePyramid();
int TestMutualInformation();
int TestTranslationSearch();
int TestAffineTransform();

int main( )
{
	// ************************************************************************ //
	// ************************************************************************ //
//	if( TestColorGradient() == -1 )												//
//	{	MessageBox( NULL, "COLOR GRADIENT FAILED", "", MB_OK ); exit(1);	  } //
//																				//
//	if( TestGradientThreshold() == -1 )											//
//	{	MessageBox( NULL, "GRADIENT THRESHOLD FAILED", "", MB_OK ); exit(1);  }	//
//																				//
//	if( TestEdgePyramid() == -1 )												//
//	{	MessageBox( NULL, "EDGE PYRAMID FAILED", "", MB_OK ); exit(1);		  }	//
//																				//
//	if( TestMutualInformation() == -1 )											//
//	{	MessageBox( NULL, "MUTUAL INFORMATION FAILED", "", MB_OK ); exit(1);  }	//
//																				//
//	if( TestTranslationSearch() == -1 )											//
//	{	MessageBox( NULL, "TRANSLATION SEARCH FAILED", "", MB_OK ); exit(1);  }	//
//																				//
//	if( TestAffineTransform() == -1 )											//
//	{	MessageBox( NULL, "AFFINE TRANSFORM FAILED", "", MB_OK ); exit(1);	  }	//
	// ************************************************************************ //
	// ************************************************************************ //
	

	// ******************************************** //
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Art Gallery\\Scene 004\\AG_S4_L_001.jpg", 1 );
	//  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Art Gallery\\Scene 004\\AG_S4_R_001.jpg", 1 );
	//
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Art Gallery\\Scene 002\\AG_S2_L_001.jpg", 1 );
	//  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Art Gallery\\Scene 002\\AG_S2_R_001.jpg", 1 );
	//
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Affine Images\\CasinoSlots_L_001.png", 1 );
	//  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Affine Images\\CasinoSlots_R_002.png", 1 );
	//
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestOriginal.bmp", 1 );
    //  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5.bmp", 1 );
	//
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestOriginal.bmp", 1 );
	//  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90.bmp", 1 );
    //
	//  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Affine Images\\CarLotOld_L_004.png", 1 );
    //  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Affine Images\\CarLotOld_R_004.png", 1 );
	//
	  cv::Mat imColorA = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Lenel\\Lenel_L_006.png", 1 );
	  cv::Mat imColorB = cv::imread( "C:\\Development\\iv\\VideoStitching\\Test Images\\Lenel\\Lenel_R_006.png", 1 );
	
	assert( imColorA.type() == CV_8UC3 );			// Input Must Be 8-Bit 3 Channel
	assert( imColorB.type() == CV_8UC3 );			// Input Must Be 8-Bit 3 Channel
	assert( imColorA.size() == imColorB.size() );	// Inputs Must Have Same Size
	

	// ************************************************	//
	int numEdge = 4;									// Gradient to Edge Quantization Levels - 1, ENTROPY + IMAGE SIZE DEPENDENT
	int numPymd = 4;									// Total Edge Gaussian Pyramid Levels (More levels == Smaller Image Size for Registration)
	int wdwRadius = 4;									// Mul. Res. Subsequent Level Window Radius
	int stepAffine = 1;	
	stepAffine = std::min<int>( stepAffine, numPymd );	// Make sure within bounds
    

	cv::Point maxMutInfoXY;								// Location of Maximum MI Value
	double maxMutInfo = 0;								// Storage for Maximum MI Value
	double timeRun = 0;									// Intialize Computation Time Variable
	
	int imRows = imColorA.rows;							// Number of Input Image(s') Rows
	int imCols = imColorA.cols;							// Number of Input Image(s') Columns
	cv::Size imSize = imColorA.size();					// Input Image(s') Size
	

	// **************************************************** //
	double scaleFrst = 0.7;									// Lower Bound of Scaling
	double scaleLast = 1.3;									// Upper Bound of Scaling
	double scaleStep = 0.05;								// Step Size in Scale Search
	
	double skewRFrst = 1;//0.9;									// Lower Bound of Row Skew
	double skewRLast = 1;//1.1;									// Upper Bound of Row Skew
	double skewRStep = 1;//0.05;								// Step Size in Row Skew Search
	
	double skewCFrst = 1;//0.9;									// Lower Bound of Col Skew
	double skewCLast = 1;//1.1;									// Upper Bound of Col Skew
	double skewCStep = 1;//0.05;								// Step Size in Col Skew Search
	
	double rotFrst = -10;			                        // Lower Bound of Rotation, in degrees
	double rotLast = 10;			                        // Upper Bound of Rotation, in degrees
	double rotStep = 0.5;									// Step Size in Rotation Search, in degrees
	
	cv::Mat tformOptimal = cv::Mat::eye( 3, 3, CV_64F );	// Result of Affine Transform Search
	double* ptrTformOptimal = (double*)tformOptimal.data;	//
	double ptrTformOptimalValues[6] = { 0,					// SCL
									    0,					// SKR
									    0,					// SKC
									    0,					// ROT
									    0,					// TRR
									    0 };				// TRC

	
	// ******************************************** //
	cv::Size imTformSize;							// Storage for Post-Transform Image Size
	cv::Mat tformAffine( 3, 3, CV_64F );			// Affine Transform Matrix
	cv::Mat tformB = cv::Mat::eye(3,3,CV_64F);		// Identity Transform Matrix
	double* ptrTformB = (double*)tformB.data;		//
	
	cv::Mat tformEye = cv::Mat::eye( 3, 3, CV_64F );//
	
	

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
	// **************************************************************** //
	cv::Mat imEnhanceA = cv::Mat::zeros( imRows, imCols, CV_8UC3 );		//
	cv::Mat imEnhanceB = cv::Mat::zeros( imRows, imCols, CV_8UC3 );		//
	
	int cleanImages = 0;												// Decision Variable

	cleanImages = stNeedClean( imColorA, imColorB );					// Check if images have disparaging color means
	
	if( cleanImages )													// If so, then "clean" images
	{
		cv::Point center(-1,-1);										// Convolution Mask Pivot Point
		cv::Size kernal( 3, 3 );										//

		cv::Mat imBlurA = cv::Mat::zeros( imRows, imCols, CV_8UC3 );	// Images have visible JPEG compression,
		cv::Mat imBlurB = cv::Mat::zeros( imRows, imCols, CV_8UC3 );	// mostly blocking artifacts. So by doing a gaussian blur first,
																		// the sharpening will still work, but the JPEG blocking won't get
																		// enhanced in the process, which would ruin the registration with noisy data.

		cv::GaussianBlur( imColorA,										// Input Image
						  imBlurA,										// Resultant Blurred Image
						  kernal,										// Kernal Size
						  0,											// Optional Std. Deviation in X
						  0,											// Optional Std. Dev. in Y
						  cv::BORDER_REFLECT );							// Border Method (Reflect avoids bleeding at bounds)

		cv::GaussianBlur( imColorB,										// Input Image
						  imBlurB,										// Resultant Blurred Image
						  kernal,										// Kernal Size
						  0,											// Optional Std. Deviation in X
						  0,											// Optional Std. Dev. in Y
						  cv::BORDER_REFLECT );							// Border Method (Reflect avoids bleeding at bounds)

		stImageEnhance( imBlurA,										// Input Image A
						imBlurB,										// Input Image B
						imEnhanceA,										// Sharpened and Color Corrected A
						imEnhanceB );									// Sharpened and Color Corrected A
	}
	else
	{
		imEnhanceA = imColorA;											// No Correction so A is shallow copied
		imEnhanceB = imColorB;											// No correction so B is shallow copied
	}


	// STEP 1: Color Gradient Maps
	// ************************************************ //
	std::vector<cv::Mat> imGradPymdA( numPymd + 1 );	// Create Storage
	std::vector<cv::Mat> imGradPymdB( numPymd + 1 );	// Create Storage
	
	stColorGradient( imEnhanceA,						//
					 imGradPymdA[0] );					//

	stColorGradient( imEnhanceB,						//
					 imGradPymdB[0] );					//
	

	// STEP 2: Multi-resolution pyramids of gradient maps
	// **************************** //
	stGradPyramid( numPymd,			//
				   imGradPymdA );	//
	
	stGradPyramid( numPymd,			//
				   imGradPymdB );	//


	// STEP 3: Affine Search
	// **************************************************** //
	for( int i = numPymd; i > numPymd - stepAffine; i-- )	// Use top of pyramid (smallest image) then step down, if applicable
    {
        int wdwTrLmt[4] = { 0, 0, 0, 0 };					// Explicit Pyramid Search Window Limits
		maxMutInfo = 0;										//

	    double tformLimits[12] = { scaleFrst,				// Set Search Limit Values
								   scaleLast,				//
								   scaleStep,				//
							       skewRFrst,				//
								   skewRLast,				//
								   skewRStep,				//
							       skewCFrst,				//
								   skewCLast,				//
								   skewCStep,				//
							       rotFrst,					//
								   rotLast,					//
								   rotStep };				//

        timeRun = (double)cv::getTickCount();				// Start Stopwatch

	    stAffineParamSearch( tformLimits,					// 12 Element Vector of Starts, Stops, and Steps
					         imGradPymdA[i],				// Input Image A [Edges, Top Of Pyramid]
						     imGradPymdB[i],				// Input Image B [Edges, Top Of Pyramid]
						     numEdge,						// Number of Edges in Images
						     tformOptimal,					// Resultant Optimal Affine Transformation
						     ptrTformOptimalValues,			// Resultant Optimal Affine Transformation
							 wdwTrLmt,						//
							 maxMutInfoXY,					// Location of Maximum MI Value
						     maxMutInfo );					// Maximum MI Value

	    timeRun = ((double)cv::getTickCount()-timeRun)		// Stop Stopwatch
				    / cv::getTickFrequency();				//

        
     // STEP 3 Results
	 // *********************************** //
	 std::cout << "Optimal Overlap Col: "	//
				  << maxMutInfoXY.x			//
				  << " Row: "				//
				  << maxMutInfoXY.y			//
				  << std::endl;				//

		std::cout << "Pyramid Level: "		//
			      << i						//
				  << " Complete in "		//
				  << timeRun				//
				  << " seconds!\n"			//
				  << std::endl;				//

		// STEP 3, if going to next level for another affine search
		// the range of the parameters is tightened and the resolution
		// is increased. (skew not used at the time, should be if used)
		// ******************************************************** //
		if( i > numPymd - stepAffine + 1 )							//
		{
			double scaleMiddle = ptrTformOptimalValues[0];			//
			double rotMiddle = ptrTformOptimalValues[3] * 180 / PI;	//
			scaleFrst = scaleMiddle - 2 * scaleStep;				//
			scaleLast = scaleMiddle + 2 * scaleStep;				//
			rotFrst = rotMiddle - 2 * rotStep;						//
			rotLast = rotMiddle + 2 * rotStep;						//
			scaleStep /= 1.5;										//
			rotStep /= 1.5;											//
			numEdge *= 3;											//
		}
    }	


	// STEP 4: Translation refinement traversing down the multi-resolution
	// pyramid to get the final, optimal translation parameters.
	// **************************************************************** //
	for( int i = numPymd-stepAffine; i > -1; i-- )						// Start at next pyramid level after search and step down
	{
    	int wdwTrLmt[4] = { 0, 0, 0, 0 };								// Explicit Pyramid Search Window Limits
		timeRun = (double)cv::getTickCount();							// Start Stopwatch

	    maxMutInfo = 0;
	    int R = maxMutInfoXY.y;											//
		int C = maxMutInfoXY.x;											//
		int pymdRows = imGradPymdA[i].rows;								//
		int pymdCols = imGradPymdA[i].cols;								//
		wdwTrLmt[0] = MAX( 2*R - wdwRadius, 0 );						// First Row Limit
		wdwTrLmt[1] = MAX( 2*C - wdwRadius, 0 );						// First Col Limit
		wdwTrLmt[2] = MIN( 2*R + wdwRadius, 2*pymdRows-1 );				// Last Row Limit
		wdwTrLmt[3] = MIN( 2*C + wdwRadius, 2*pymdCols-1 );				// Last Col Limit
		
		double tformLimits[12] = { ptrTformOptimalValues[0],			// Set search limits to only search the current, optimal transform.
								   ptrTformOptimalValues[0],			// This is not an affine search anymore, it is just a translation search.
								   1,									// Instead of copying 2/3 of the code of stAffineParamSearch and pasting it here,
							       ptrTformOptimalValues[1],			// we call it again, just with restricted search bounds.
								   ptrTformOptimalValues[1],			//
								   1,									//
							       ptrTformOptimalValues[2],			//
								   ptrTformOptimalValues[2],			//
								   1,									//
							       ptrTformOptimalValues[3] * 180 / PI,	//
								   ptrTformOptimalValues[3] * 180 / PI,	//
								   1 };									//

        stAffineParamSearch( tformLimits,								// 12 Element Vector of Starts, Stops, and Steps
					         imGradPymdA[i],							// Input Image A [Edges, Top Of Pyramid]
						     imGradPymdB[i],							// Input Image B [Edges, Top Of Pyramid]
						     numEdge,									// Number of Edges in Images
						     tformOptimal,								// Resultant Optimal Affine Transformation
						     ptrTformOptimalValues,						// Resultant Optimal Affine Transformation
							 wdwTrLmt,									// Limits for search, restricted to only compensate for downsampling
							 maxMutInfoXY,								// Location of Maximum MI Value
						     maxMutInfo );								// Maximum MI Value


		timeRun = ((double)cv::getTickCount()-timeRun)					// Stopwatch Stop
					/ cv::getTickFrequency();							//
		
		//
		// ************************************ //
		std::cout << "Optimal Overlap Col: "	//
				  << maxMutInfoXY.x				//
				  << " Row: "					//
				  << maxMutInfoXY.y				//
				  << std::endl;					//

		std::cout << "Pyramid Level: "			//
				  << i							//
				  << " Complete in "			//
				  << timeRun					//
				  << " seconds!\n"				//
				  << std::endl;					//
	}
	

	// RESULTS
	// **************************************************** //
	std::string paramName[6];								// Create string for display of Optimal Parameters
	paramName[0] = "SCL: ";									// Scale
	paramName[1] = "SKR: ";									// Skew Rows
	paramName[2] = "SKC: ";									// Skew Cols
	paramName[3] = "ROT: ";									// Rotation
	paramName[4] = "TRR: ";									// Translation Rows
	paramName[5] = "TRC: ";									// Translation Cols

	double printParam = 0;									//

	for( int i = 0; i < 6; i++ )							// Display Optimal Parameters
	{
		std::cout << paramName[i]							//
				  << ( printParam = (i == 3)				//
					  ? ptrTformOptimalValues[i]*180/PI		//
					  : ptrTformOptimalValues[i] )			//
				  << " "									//
				  << std::endl;								//
	}

	
	// RESULTS
	// **************************************** //
	int TRR = (int)(ptrTformOptimalValues[4]);	// Affine Row Translation Parameter
	int TRC = (int)(ptrTformOptimalValues[5]);	// Affine Col Translation Parameter
	
	ptrTformOptimalValues[4] = 0;				// Set to zero so images can be centered before translated.
	ptrTformOptimalValues[5] = 0;				// Translation parameters only make sense for centered images.

	cv::Mat tformA = tformOptimal.clone();		//
	double* ptrTformA = (double*)tformA.data;	//


    

	// INITIAL IMAGE TRANSFORMATION
	// Images are first transformed and centered, to match the search
	// conditions. Then the translation is applied before they can
	// be stitched together.
	// ************************************************************ //
	stTformSize( imSize,											//
				 imTformSize,										//
				 tformA );											//

	cv::Size imTformSizeOld = imTformSize;							//
	
	imTformSize.width = ( imTformSizeOld.width > imSize.width )		//
						? imTformSizeOld.width						//
						: imSize.width;								//

	imTformSize.height = ( imTformSizeOld.height > imSize.height )	//
						 ? imTformSizeOld.height					//
						 : imSize.height;							//
				
	
	imTformSize.width += 2 * abs( TRC );							// This sets the resultant width to the appropriate size regardless of image location.
	imTformSize.height += 2 * abs( TRR );							// This means that Image B will be centered completely, and image A will be in it's proper
																	// position, with extraneous black pixels on the opposite sides of B.
	
	int worldRows = (int)( imTformSize.height );					//
	int worldCols = (int)( imTformSize.width );						//
	
	
	// CENTERED AND SIZE CORRECTED IMAGE TRANSFORMATIONS
	// tformB will always have 0s at 2 and 5, but tformA could already have values that shouldn't be overwritten
	// **************************************************************************************** //
	ptrTformA[2] += ROUND( POS( (imTformSize.width - imTformSizeOld.width) ) / 2. ) + TRC;		//
	ptrTformA[5] += ROUND( POS( (imTformSize.height - imTformSizeOld.height) ) / 2. ) + TRR;	//
	
	ptrTformB[2] = ROUND( POS( (imTformSize.width - imColorB.cols) ) / 2. );					//
	ptrTformB[5] = ROUND( POS( (imTformSize.height - imColorB.rows) ) / 2. );					//
	
	
	
	// COLOR IMAGE TRANSFORMATIONS
	// ******************************************** //
	cv::Mat imWorldA = cv::Mat::zeros( worldRows,	//
									   worldCols,	//
									   CV_8UC3 );	//

	cv::Mat imWorldB = cv::Mat::zeros( worldRows,	//
									   worldCols,	//
									   CV_8UC3 );	//

	cv::warpPerspective( imEnhanceA,				//
						 imWorldA,					//
						 tformA,					//
						 imWorldA.size(),			//
						 cv::INTER_LINEAR,			//
						 cv::BORDER_TRANSPARENT );	//
	
	cv::warpPerspective( imEnhanceB,				//
						 imWorldB,					//
						 tformB,					//
						 imWorldB.size(),			//
						 cv::INTER_LINEAR,			//
						 cv::BORDER_TRANSPARENT );	//

	
	// CENTERED AND SIZE CORRECTED GRADIENT TRANSFORMATIONS
	// ******************************************************** //
	cv::Mat imGradA = cv::Mat::zeros( worldRows,				//
									  worldCols,				//
									  imGradPymdA[0].type() );	//

	cv::Mat imGradB = cv::Mat::zeros( worldRows,				//
									  worldCols,				//
									  imGradPymdB[0].type() );	//

	cv::warpPerspective( imGradPymdA[0],						//
						 imGradA,								//
						 tformA,								//
						 imGradA.size(),						//
						 cv::INTER_CUBIC,						//
						 cv::BORDER_TRANSPARENT );				//
	
	cv::warpPerspective( imGradPymdB[0],						//
						 imGradB,								//
						 tformB,								//
						 imGradB.size(),						//
						 cv::INTER_CUBIC,						//
						 cv::BORDER_TRANSPARENT );				//
	
	// CENTERED AND SIZE CORRECTED GRADIENT MASK TRANSFORMATIONS
	// ******************************************************** //
	cv::Mat tempA = cv::Mat::ones( imEnhanceA.size(), CV_8U );	//
	cv::Mat tempB = cv::Mat::ones( imEnhanceB.size(), CV_8U );	//

	cv::Mat imMaskA = cv::Mat::zeros( imGradA.size(), CV_8U );	//
	cv::Mat imMaskB = cv::Mat::zeros( imGradB.size(), CV_8U );	//

	cv::warpPerspective( tempA,									//
						 imMaskA,								//
						 tformA,								//
						 imGradA.size(),						//
						 cv::INTER_NEAREST,						//
						 cv::BORDER_TRANSPARENT );				//
	
	cv::warpPerspective( tempB,									//
						 imMaskB,								//
						 tformB,								//
						 imGradB.size(),						//
						 cv::INTER_NEAREST,						//
						 cv::BORDER_TRANSPARENT );				//


	double minA, maxA;											//
	double minB, maxB;											//

	cv::minMaxLoc( imGradA,										//
				   &minA,										//
				   &maxA,										//
				   0,											//
				   0,											//
				   imMaskA );									// Find and Store Extreme Values only in Mask

	cv::minMaxLoc( imGradB,										//
				   &minB,										//
				   &maxB,										//
				   0,											//
				   0,											//
				   imMaskB );									// Find and Store Extreme Values only in Mask

	float* ptrGradA = (float*)imGradA.data;						//
	float* ptrGradB = (float*)imGradB.data;						//
	uchar* ptrMaskA = imMaskA.data;								//
	uchar* ptrMaskB = imMaskB.data;								//


	// Normalize the gradient image within the mask region to prepare for
	// quantization
	// ************************************************************************************ //
	for( int i = 0; i < worldRows*worldCols; i++ )											// Normalize Map by Maximum and
	{
		if( ptrMaskA[i] > 0 )																//
		{
			ptrGradA[i] = (float)ROUND( 255.*((ptrGradA[i]-minA)/(maxA-minA)) );	//
			ptrGradA[i] = ( ptrGradA[i] == 0 )									//
							   ? 1															//
							   : ptrGradA[i];											// Quantize to Integers in [1,255]
		}

		if( ptrMaskB[i] > 0 )																//
		{
			ptrGradB[i] = (float)ROUND( 255.*((ptrGradB[i]-minB)/(maxB-minB)) );	//
			ptrGradB[i] = ( ptrGradB[i] == 0 )									//
							   ? 1															//
							   : ptrGradB[i];											// Quantize to Integers in [1,255]
		}
	}

	cv::Mat imGradWorldA = cv::Mat::zeros( worldRows, worldCols, CV_8U );													//
	cv::Mat imGradWorldB = cv::Mat::zeros( worldRows, worldCols, CV_8U );													//
	
	imGradA.convertTo(imGradWorldA, CV_8U, 1, 0 );											//
	imGradB.convertTo(imGradWorldB, CV_8U, 1, 0 );											//
	
	
	// GET CORRESPONDENCE FEATURES
	// **************************************************** //
	cv::Mat tformProjAB = cv::Mat::zeros( 3, 3, CV_64F );	//
	
	// Vector size is the upper limit to how many features will be returned.
	// Currently set to semi-arbitrary 400, but for larger or smaller images,
	// this could change. The other restrictions on features are from
	// the non-maxima suppression, distance, and harris metric. So if 1000 pixels
	// meet those criteria, the best 400 (meaning the 400 with the highest
	// "cornerness") are returned for the correspondence search.
	// The more features there are, the worse some of them will be, and ultimately
	// the more likely they are to be outliers and to skew and destroy the
	// RANSAC optimization to automatically find the appropriate homography.
	// The overlap region has been restricted to be at least 30% of the total
	// possible per-pixel overlap, and will only be a good overlap if there
	// is high entropy in the scene. So a scene with high entropy could approach
	// the 30% bound and be good, but a low entropy scene needs more overlap.
	// And higher entropy scenes mean most-likely more corner features, but
	// a low entropy repeated pattern scene could have lots of corners too.
	// And with a high entropy scene, the results should be better because
	// the mutual information won't be easily tricked from its spatial
	// independence, but low entropy scenes (repeated patterns) could have
	// multiple mathematically valid matches, and give poor results.
	// **************************************************** //
	std::vector<cv::Point2f> pxOvlpCornerA( 400 );			//
	std::vector<cv::Point2f> pxOvlpCornerB( 400 );			//
	
	stOverlapFeatures( imWorldA,							//
					   imWorldB,							//
					   pxOvlpCornerA,						//
					   pxOvlpCornerB );						//

	stCorrespondFeatures( imGradWorldA,						//
						  imGradWorldB,						//
						  pxOvlpCornerA,					//
						  pxOvlpCornerB,					//
						  tformProjAB );					//
	

	
	// APPLY PROJECTIVE TRANSFORM TO IMAGES AND PASTE B ONTO A
	// **************************************************** //
	cv::Mat imWorldProj = cv::Mat::zeros( worldRows,		//
										  worldCols,		//
										  CV_8UC3 );		//

	cv::Mat imWorldAffn = cv::Mat::ones( worldRows,			//
										 worldCols,			//
										 CV_8UC3 );			//

	cv::warpPerspective( imWorldA,							//
						 imWorldProj,						//
						 tformProjAB,						//
						 imWorldProj.size(),				//
						 cv::INTER_LINEAR,					//
						 cv::BORDER_TRANSPARENT );			//

	cv::warpPerspective( imEnhanceB,						//
						 imWorldProj,						//
						 tformB,							//
						 imWorldProj.size(),				//
						 cv::INTER_LINEAR,					//
						 cv::BORDER_TRANSPARENT );			//
	

	cv::warpPerspective( imColorA,							//
						 imWorldAffn,						//
						 tformA,							//
						 imWorldAffn.size(),				//
						 cv::INTER_CUBIC,					//
						 cv::BORDER_TRANSPARENT );			//

	
	cv::warpPerspective( imColorB,							//
						 imWorldAffn,						//
						 tformB,							//
						 imWorldAffn.size(),				//
						 cv::INTER_CUBIC,					//
						 cv::BORDER_TRANSPARENT );			//


	// COLOR IMAGE PROJECTIVE TRANSFORMATION
	// ************************************************ //
	cv::Mat imProjWorldA = cv::Mat::zeros( worldRows,	//
										   worldCols,	//
										   CV_8UC3 );	//

	cv::warpPerspective( imWorldA,						//
						 imProjWorldA,					//
						 tformProjAB,					//
						 imWorldA.size(),				//
						 cv::INTER_LINEAR,				//
						 cv::BORDER_TRANSPARENT );		//
	
	
	
	// WRITE IMAGES TO DISC
	// ************************************************ //
	cv::imwrite( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90_WA.jpg", imWorldA );
	cv::imwrite( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90_WAp.jpg", imProjWorldA );
	cv::imwrite( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90_WB.jpg", imWorldB );
	cv::imwrite( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90_WABaff.jpg", imWorldAffn );
	cv::imwrite( "C:\\Development\\iv\\VideoStitching\\Test Images\\LenelPerfectAffineTests\\TestRotated5Scaled90_WAB.jpg", imWorldProj );
	
	// Exit ************ //
	return 0;
}


int TestColorGradient()
{
	return 0;
}

int TestGradientThreshold()
{
	return 0;
}


int TestEdgePyramid()
{
	return 0;
}


int TestMutualInformation()
{
	return 0;
}

int TestTranslationSearch()
{
	/*cv::Mat testImageA( 3, 4, CV_8U );
	cv::Mat testImageB( 3, 4, CV_8U );

	int valueA[12] = { 1, 1, 3, 1, 3, 2, 2, 2, 1, 3, 3, 1 };
	int valueB[12] = { 3, 2, 2, 2, 1, 2, 3, 1, 2, 1, 2, 1 };
		
	int CHECK[2] = {2, 3};

	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			int index = i+j*3;
			testImageA.data[index] = valueA[index];
			testImageB.data[index] = valueB[index];
		}
	}

	int RESULT[2] = {0, 0};
	
	int wdwTrLim[1] = {0};

	int mapSize = (2*testImageA.rows - 1)*(2*testImageA.cols - 1);

	stTranslateSearch( testImageA, testImageB, 3, wdwTrLim, RESULT[0], RESULT[1], mapSize );

	std::cout << RESULT[0] << " " << RESULT[1] << std::endl;

	if( (CHECK[0] != RESULT[0]) | (CHECK[1] != RESULT[1]) )
		return -1;*/

	return 0;
}


int TestAffineTransform()
{
	return 0;
}
