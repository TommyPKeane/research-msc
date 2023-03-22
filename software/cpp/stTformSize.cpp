#include "stStitchingParams.h"

// Code copied, with some minor modifications, from the VideoStitching dll

void TransformPoint( const int& x,
					 const int& y,
					 const double tform[9],
					 int &tX,
					 int &tY )
{
	// ******************************************************** //
	double tZ = x*tform[6] + y*tform[7] + tform[8];				// Transformed Z Value, 1 if affine
	
	tX = (int)ROUND( (x*tform[0] + y*tform[1] + tform[2])/tZ );	// Transformed X value (Col)
	tY = (int)ROUND( (x*tform[3] + y*tform[4] + tform[5])/tZ );	// Transformed Y value (Row)
}




void stTformSize( const cv::Size& imSize,
				  cv::Size& imTformSize,
				  cv::Mat& tformMat )
{
	// Code Copied and Modified from:
	//		DetermineOutputBoundingRectangle()

	// Goes through all of the transformation images and determines their boundaries
    // Code taken from Core::TransformCalculator.c and modified
    // Run the translation over the boundary points in all images and determine the extremes
    
	// ******************************************** //
	int min_x = MAXINT;								// Intialize Minimum to Largest Possible Integer
    int max_x = MININT;								// Intialize Minimum to Largest Possible Integer
    int min_y = MAXINT;								// Intialize Minimum to Largest Possible Integer
    int max_y = MININT;								// Intialize Minimum to Largest Possible Integer
    int x;											// Transformed X Location (Col)
    int y;											// Transformed Y Location (Row)
	double* ptrTformMat = (double*)tformMat.data;	// Pointer to transform data
    
    
	// ******************************************************************** //
    TransformPoint( 0, 0, ptrTformMat, x, y );								// Transform Top Left Corner
	min_x = x < min_x ? x : min_x;											// Check if X is new Minimum
	max_x = x > max_x ? x : max_x;											// Check if X is new Maximum
	min_y = y < min_y ? y : min_y;											// Check if Y is new Minimum
	max_y = y > max_y ? y : max_y;											// Check if Y is new Maximum
    
	TransformPoint( imSize.width-1, imSize.height-1, ptrTformMat, x, y );	// Transform Bottom Right Corner
    min_x = x < min_x ? x : min_x;											// Check if X is new Minimum
	max_x = x > max_x ? x : max_x;											// Check if X is new Maximum
	min_y = y < min_y ? y : min_y;											// Check if Y is new Minimum
	max_y = y > max_y ? y : max_y;											// Check if Y is new Maximum
    
	TransformPoint( imSize.width-1, 0, ptrTformMat, x, y );					// Transform Top Right Corner
    min_x = x < min_x ? x : min_x;											// Check if X is new Minimum
	max_x = x > max_x ? x : max_x;											// Check if X is new Maximum
	min_y = y < min_y ? y : min_y;											// Check if Y is new Minimum
	max_y = y > max_y ? y : max_y;											// Check if Y is new Maximum
    
	TransformPoint( 0, imSize.height-1, ptrTformMat, x, y );				// Transform Bottom Left Corner
    min_x = x < min_x ? x : min_x;											// Check if X is new Minimum
	max_x = x > max_x ? x : max_x;											// Check if X is new Maximum
	min_y = y < min_y ? y : min_y;											// Check if Y is new Minimum
	max_y = y > max_y ? y : max_y;											// Check if Y is new Maximum
    
    // Adding 1 takes care of digital counting difference
	// i.e. max=10, min=9, image is 2 pixels wide, but 10 - 9 = 1;
	// Need that extra addition to fix that.
	imTformSize.height = ( max_y - min_y ) + 1;								// Determine Height of Affine Transformed image (Row)
    imTformSize.width = ( max_x - min_x ) + 1;								// Determine Width of Affine Transformed image (Col)

    ptrTformMat[2] -= min_x;												// Shift Transform Parameter (Col)
    ptrTformMat[5] -= min_y;												// Shift Transform Parameter (Row)
}
