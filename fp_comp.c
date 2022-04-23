#include "fp_comp.h"

float machEps(void)
{
	float e = 1.0f;

	while (1.0f + e / 2.0f > 1.0f)
		e /= 2.0f;
	return e;
}

#ifdef MY_COMP_FLOAT
/**
 * My float comparison function
 * is worse than AlmostEqual2sComplement
 * for values near zero, + ulp does not work so well
 */

bool AlmostEqual2sComplement(float a, float b, int ulp)	   // units in the last place
{
	float diff = fabsf(a - b);

	#ifdef _DEBUG
	printf(
		"a=%g, b=%g, diff=%g, larg=%g, eps=%g, res=%d\n",
		a,
		b,
		diff,
		fmaxf(fabsf(a), fabsf(b)),
		macheps(),
		(fabsf(a - b) <= fmaxf(fabsf(a), fabsf(b)) * macheps() * (float)ulp));
	#endif
	return (fabsf(a - b) <= fmaxf(fabsf(a), fabsf(b)) * macheps() * (float)ulp);
}
#else
/**
 * Float comparison function (calculation of ulp difference) using integer variables
 * Based on an article by Bruce Davison - Comparing Floating Point Numbers, 2012 Edition
 * https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
 * https://habr.com/ru/post/112953/
 */

	#ifndef NOT_USE_OVERFLOW
/**
 * The float order is the same as the sign-and-magnitude integer order.
 * While processors operate two's complement integer.
 * To find the distance between two floats in ulp,
 * you need to translate one into the other
 * and this is done via 0x80000000 - paint for negative floats.
 *
 * Truncating the sign bit via &= 0x7fffffff;
 * will cause the function to consider signed numbers equal
 * (for example +1 will be equal to -1)
 *
 * Plus, there are also wonderful rakes that Bruce Dawson has.
 * int intDiff must be an unsigned int,
 * then an unsigned comparison will be used
 * (according to the rules of comparison With),
 * otherwise if an overflow occurs in abs,
 * its result will be interpreted as negative
 * and if (intDiff <= maxUlps) will return true for completely different numbers.
 */

bool AlmostEqual2sComplement(float A, float B, int maxUlps)
{
	if (sizeof(int) != sizeof(float))
	{
		my_log(
			true,
			error,
			__LINE__,
			"AlmostEqual2sComplement return false,"
			"sizeof(int)=%zu != sizeof(float)=%zu\n",
			sizeof(int),
			sizeof(float));
		return false;
	}

	// maxUlps should not be negative and not too large
	// so that NaN is not equal to any number
	if (maxUlps > 0 && maxUlps < 4 * 1024 * 1024)
	{
		int aInt = *(int*)&A;
		// Remove the sign in aInt, if any, to get a properly ordered sequence
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// NOT aInt &= 0x7fffffff;
		/// Narrowing conversion from 'unsigned int'
		/// to signed type 'int' is implementation-defined

		// Similarly for bInt
		int bInt = *(int*)&B;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		unsigned int intDiff = abs(aInt - bInt);
		#ifdef _DEBUG
		printf("a=%g, b=%g, dif=%u, ulp=%d, res=%d\n", A, B, intDiff, maxUlps, intDiff <= maxUlps);
		#endif
		if (intDiff <= maxUlps)
			return true;
	}
	else
	{
		my_log(true,
			   error,
			   __LINE__,
			   "AlmostEqual2sComplement return false,"
			   "maxUlps = %d > 4 * 1024 * 1024 || == 0\n",
			   maxUlps);
	}

	return false;
}

	#else
/**
 * BUT
 *
 * int aInt = *(int*)&A;
 * if (aInt < 0) aInt = 0x80000000 - aInt;
 * -> Clang-Tidy: Narrowing conversion from 'unsigned int'
 * to signed type 'int' is implementation-defined
 * I brought out the meaning of clang's remark about "Narrowing conversion",
 * and did not use the unsigned int overflow
 */
bool AlmostEqual2sComplement(float A, float B, int maxUlps)
{
	if (sizeof(int) != sizeof(float))
	{
		my_print_err(
			error,
			__LINE__,
			"AlmostEqual2sComplement return false,"
			"sizeof(int)=%zu != sizeof(float)=%zu\n",
			sizeof(int),
			sizeof(float));
		return false;
	}

	// maxUlps should not be negative and not too large
	// so that NaN is not equal to any number
	if (maxUlps > 0 && maxUlps < 4 * 1024 * 1024)
	{
		unsigned int aInt = *(int*)&A;
		// Remove the sign in aInt, if any, to get a properly ordered sequence
		if (aInt > 0x80000000)
			aInt = 0xFFFFFFFF - (aInt - 0x80000000) + 1;

		// Similarly for bInt
		unsigned int bInt = *(int*)&B;
		if (bInt > 0x80000000)
			bInt = 0xFFFFFFFF - (bInt - 0x80000000) + 1;

		unsigned int intDiff = 0;
		printf("%u %u\n", aInt, bInt);
		if (aInt > bInt)
			intDiff += (aInt - bInt);
		else if (aInt < bInt)
			intDiff += (bInt - aInt);
		else
			intDiff = 0;

		if ((aInt > 0x80000000 && bInt < 0x80000000) || ((aInt < 0x80000000 && bInt > 0x80000000)))
			intDiff = 0xFFFFFFFF - intDiff + 1;

		#ifdef _DEBUG
		printf("a=%g, b=%g, dif=%u, ulp=%d, res=%d\n", A, B, intDiff, maxUlps, intDiff <= maxUlps);
		#endif
		if (intDiff <= maxUlps)
			return true;
	}
	else
	{
		my_print_err(
			error,
			__LINE__,
			"AlmostEqual2sComplement return false,"
			"maxUlps = %d > 4 * 1024 * 1024 || == 0\n",
			maxUlps);
	}

	return false;
}
	#endif	  // NOT_USE_OVERFLOW

#endif	  // MY_COMP_FLOAT