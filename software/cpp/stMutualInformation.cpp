#include "stStitchingParams.h"

//
// Mutual Information Formula:
// I(A,B) = SUM( SUM( p(A,B) * log2( p(A,B) / ( p(A)*p(B) ) ) ) );
//

//
// Entropy Formula:
// H(A) = - SUM( p(A) * log2( p(A) ) );
//


void stMutualInformation( const int* histA,
						  const int* histB,
						  const int* histAB,
						  const int& pixNorm,
						  const int& numEdge,
						  double& mutInfo,
						  double& entropyA,
						  double& entropyB )
{
	// Check for Trivial Results
	// ************************************ //
	if( (!pixNorm) )						// Avoid Division By Zero
	{
		return;								// Comes from trivial overlaps where there are only 0s (invalid data)
	}
	
	// Calculate Mutual Information and Entropy
	// ******************************************************************************** //
	for( int i = 0; i < (numEdge+1); i++ )												// Loop Through Bivariate Hist Rows
	{
		entropyA += ( (((double)histA[i])/pixNorm) * LOG2( ((double)histA[i])/pixNorm) );	// Entropy of A
		entropyB += ( (((double)histB[i])/pixNorm) * LOG2( ((double)histB[i])/pixNorm) );	// Entropy of B

		for( int j = 0; j < (numEdge+1); j++ )											// Loop through Bivariate Hist Columns
		{
			double distAB = ((double)histAB[j + (numEdge+1)*i])/pixNorm;					// Joint Distribution Value

			double margProd = (((double)histA[i])/pixNorm) * (((double)histB[j])/pixNorm);	// Product of Marginal Distribution Values

			if( margProd == 0 )															// Trivial Result Condition
				continue;																// Avoids Division by Zero

			double ratioJointMarg = ( distAB / margProd );								// Ratio of Joint to Marginal Product
			
			mutInfo += ( distAB * LOG2( ratioJointMarg ) );								// Mutual Information Calculation
		}
	}

	// Correct Entropies
	// **************** //
	entropyB *= -1;		// Definition of Entropy is Negative
	entropyA *= -1;		// Results Are >= 0 Though

	
	// END OF FILE
	// ************ //
	return;			// Returns Mutual Information and Entropies By Reference
}
