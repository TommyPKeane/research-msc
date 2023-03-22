//
// Main file for includes and global functions.
// Called by stStitchingParams.h, all other functions
//  call stStitchingParams.h.
//

#include <cv.h>			// OpenCV
#include <cxcore.h>		// OpenCV
#include <highgui.h>	// OpenCV

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <limits>

// DEFINE
// **************************************************** //

// Definition of Pi
#define PI	3.14159265358979323

// (Unit Step Product) Checks if input is Positive,
//  returns input if true, zero if false
#define POS(x) ( (x) > 0 ? (x) : 0 )

// Base 2 Logarithm if input is non-zero,
//  returns zero for a zero input
//  Essential to Mutual Information and Homography Calculations for Discrete Data
#define LOG2(x) ( ((x)!=0) ? (log((x))/log(2.)) : 0 )

// Mathematical Rounding Function
#define ROUND(x) ( floor( (x) + 0.5 ) )
