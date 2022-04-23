#ifndef FP_COMP_H
#define FP_COMP_H

//#define MY_COMP_FLOAT
//#define NOT_USE_OVERFLOW

#include "my_logging.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

float machEps(void);
bool AlmostEqual2sComplement(float A, float B, int maxUlps);

#ifdef MY_COMP_FLOAT
	/// My comparison implementation
#else
	#ifndef NOT_USE_OVERFLOW
		/// Implementation of the comparison based on the article by Bruce Davison
	#else
		/// My thoughts on fixing the "Narrowing conversion" warning
		// from the previous implementation
	#endif	  // NOT_USE_OVERFLOW
#endif		  // MY_COMP_FLOAT

#endif	  // FP_COMP_H
