#include "gauss_jordan_elimination_algorithm.h"

void swap(size_t *a, size_t *b)
{
	size_t buf = *a;
	*a = *b;
	*b = buf;
}

size_t pre_pivoting(float **matrix, const size_t n)
{
	int n_unknown = 0;
	float maxi;
	int line_maxi;
	do
	{
		float a = matrix[n_unknown][n_unknown];
		maxi = a;
		line_maxi = n_unknown;

		for (int i = n_unknown + 1; i < n; i++)
		{
			a = matrix[i][n_unknown];
			if (fabsf(a) > fabsf(maxi))
			{
				maxi = a;
				line_maxi = i;
			}
		}

		n_unknown++;
	} while (maxi == 0.0f && n_unknown < n);
	/// I believe that with scanf("%f") there is no margin of error

	swap((size_t *)&matrix[n_unknown - 1], (size_t *)&matrix[line_maxi]);

#ifdef _DEBUG
	printf("swap %d %d\n", n_unknown - 1, line_maxi);
	printf("c=%d\n", n_unknown);
	my_print_matrix("Matrix after pivoting", __LINE__, matrix, n, n + 1);
#endif

	return n_unknown - 1;
}

float smart_sub(Element *element, const float current, const float subtracted, const float piv, const float multiplied)
{
	element->left = (double)current * (double)piv;
	element->right = (double)subtracted * (double)multiplied;

	float result = (float)((element->left - element->right) / (double)piv);

#ifdef _DEBUG
	printf(
		"lr1: %d %d %d, %d %d %d\n",
		AlmostEqual2sComplement((float)element->left, 0.0f, MAX_ULP),
		current != 0.0f,
		piv != 0.0f,
		AlmostEqual2sComplement((float)element->right, 0.0f, MAX_ULP),
		subtracted != 0.0f,
		multiplied != 0.0f);
#endif

	if ((AlmostEqual2sComplement((float)element->left, 0.0f, MAX_ULP) && current != 0.0f && piv != 0.0f) ||
		(AlmostEqual2sComplement((float)element->right, 0.0f, MAX_ULP) && subtracted != 0.0f && multiplied != 0.0f))
	{
#ifdef _DEBUG
		printf("left, right swap1\n");
#endif

		element->left = (double)current / (double)multiplied;
		element->right = (double)subtracted / (double)piv;

		result = (float)((element->left - element->right) * (double)multiplied);

		if (AlmostEqual2sComplement((float)element->left, 0.0f, MAX_ULP) ||
			AlmostEqual2sComplement((float)element->right, 0.0f, MAX_ULP) || AlmostEqual2sComplement(multiplied, 0.0f, MAX_ULP))
		{
			element->left = (double)current;
			element->right = (double)subtracted * (double)multiplied / (double)piv;

			if (AlmostEqual2sComplement((float)element->right, 0.0f, MAX_ULP))
			{
				element->right = (double)subtracted * (double)multiplied / (double)piv;
			}

			result = (float)(element->left - element->right);
		}
	}
	return result;
}

void pivoting(float **matrix, Element *maxi, const size_t n, const size_t prev_piv_c, const size_t n_unknown)
{
#ifdef _DEBUG
	printf("c=%d piv=%d\n", n_unknown, prev_piv_c);
	printf("2*) %g - (%g/%g * %g)\n",
		   matrix[n_unknown][n_unknown],
		   matrix[prev_piv_c][n_unknown],
		   matrix[prev_piv_c][prev_piv_c],
		   matrix[n_unknown][prev_piv_c]);
#endif

	/*
	 * comparison by "smart_sub"
	 * gives a better result than by formula (%g <> (%g/%g * %g))
	 */

	maxi->line = n_unknown;
	matrix[n_unknown][n_unknown] =
		smart_sub(maxi, matrix[n_unknown][n_unknown], matrix[prev_piv_c][n_unknown], matrix[prev_piv_c][prev_piv_c], matrix[n_unknown][prev_piv_c]);

	for (size_t i = n_unknown + 1; i < n; i++)
	{
		Element current;
		current.line = i;
		matrix[i][n_unknown] =
			smart_sub(&current, matrix[i][n_unknown], matrix[prev_piv_c][n_unknown], matrix[prev_piv_c][prev_piv_c], matrix[i][prev_piv_c]);
#ifdef _DEBUG
		printf("2**) %g - (%g/%g * %g)\n", matrix[i][n_unknown], matrix[prev_piv_c][n_unknown], matrix[prev_piv_c][prev_piv_c], matrix[i][prev_piv_c]);
#endif
		if (fabs(maxi->left - maxi->right) < fabs(current.left - current.right))
			(*maxi) = current;
	}
}

size_t pivoting_and_calculating_column(float **matrix, const size_t n, const size_t prev_piv_c, size_t n_unknown)
{
	Element maxi;	 // structure for finding the maximum element when subtracting
	/*
	 * it is necessary to look for the next pivot element when subtracting,
	 * since it will not work adequately to compare float with 0,
	 * provided that float was calculated in the program
	 */

	do
	{
		pivoting(matrix, &maxi, n, prev_piv_c, n_unknown);

		if (AlmostEqual2sComplement((float)maxi.left, (float)maxi.right, MAX_ULP))
		{
			/*
			 * if the comparison of the left and right parts of the maximum element returns true,
			 * we skip the current row (+ column, automatically, obviously it is already zero)
			 */
#ifdef _DEBUG
			printf("swap %d %d\n", n_unknown, maxi.line);
#endif
			swap((size_t *)&matrix[n_unknown], (size_t *)&matrix[maxi.line]);
			matrix[n_unknown][n_unknown] = 0.0f;
			/*
			 * we reset the reference element to further understand that we missed this line,
			 * to calculate the possibility of solving the SoLE (System of linear equations)
			 */

			my_print_matrix("Matrix after skipping in for", __LINE__, matrix, n, n + 1);

			if (n_unknown != n - 1)
			{
				for (size_t j = n_unknown + 1; j < n + 1; j++)
				{
#ifdef _DEBUG
					printf("2***) %g - (%g/%g * %g)\n",
						   matrix[n_unknown][j],
						   matrix[prev_piv_c][j],
						   matrix[prev_piv_c][prev_piv_c],
						   matrix[n_unknown][prev_piv_c]);
#endif

					Element current;
					matrix[n_unknown][j] =
						smart_sub(&current, matrix[n_unknown][j], matrix[prev_piv_c][j], matrix[prev_piv_c][prev_piv_c], matrix[n_unknown][prev_piv_c]);
				}
				n_unknown++;
			}
			else
				break;
		}
		else
			break;
	} while (n_unknown < n);

	swap((size_t *)&matrix[n_unknown], (size_t *)&matrix[maxi.line]);

#ifdef _DEBUG
	printf("swap %d %d\n", n_unknown, maxi.line);
	printf("c=%d\n", n_unknown);
#endif

	my_print_matrix("Matrix after pivoting in for", __LINE__, matrix, n, n + 1);
	return n_unknown;
}

void calculating_remaining_matrix(float **matrix, const size_t n, const size_t prev_piv_c, const size_t n_unknown)
{
	for (size_t i = n_unknown; i < n; i++)
	{
		for (size_t j = n_unknown + 1; j < n + 1; j++)
		{
#ifdef _DEBUG
			printf("2) %g - (%g/%g * %g)\n", matrix[i][j], matrix[i][prev_piv_c], matrix[prev_piv_c][prev_piv_c], matrix[prev_piv_c][j]);
#endif

			Element current;
			matrix[i][j] =
				smart_sub(&current, matrix[i][j], matrix[i][prev_piv_c], matrix[prev_piv_c][prev_piv_c], matrix[prev_piv_c][j]);
		}
	}
	my_print_matrix("Matrix in for", __LINE__, matrix, n, n + 1);
}

size_t forward_elimination(float **matrix, const size_t n)
{
	/*
	 * the first non-zero element is found (the maximum in the column) and is placed on the main diagonal,
	 * the search is carried out in order of increasing column indexes, the zero column is the leftmost,
	 * the return value is the pivot elementfor further zeroing of the following rows
	 */
	size_t prev_piv_c = pre_pivoting(matrix, n);
	size_t n_independent_variable = prev_piv_c;
	/*
	 * the position of the pivot element can be judged by how many zero columns appeared in the original matrix,
	 * therefore it is possible to understand the number of independent variables up to the first pivot element
	 */

	for (size_t n_unknown = prev_piv_c + 1; n_unknown < n; n_unknown++)
	{
		size_t new_prev_piv_c;	  // new pivot element

		n_unknown = pivoting_and_calculating_column(matrix, n, prev_piv_c, n_unknown);

		/*
		 * if the comparison of the left and right parts of the maximum element returns false,
		 * we write the following pivot element
		 */
		new_prev_piv_c = n_unknown;
		if (matrix[n_unknown][n_unknown] == 0.0f)
			new_prev_piv_c++;
		n_independent_variable += (new_prev_piv_c - prev_piv_c - 1);	// increasing the number of independent variable

		calculating_remaining_matrix(matrix, n, prev_piv_c, n_unknown);

		prev_piv_c = new_prev_piv_c;
	}
	my_print_matrix("Matrix after for", __LINE__, matrix, n, n + 1);

	return n_independent_variable;
}

Answer back_substitution(float **matrix, const size_t n, const size_t n_independent_variable)
{
	Answer result = { n_independent_variable == 0, true };

	if (n_independent_variable > 0)	   // if SoLE (System of linear equations) not has one solution
	{
#ifdef _DEBUG
		printf("niv = %d\n", n_independent_variable);
#endif

		for (size_t i = n; i--;)
		{
			double result_unknown = 0;
			for (size_t j = i + 1; j < n; j++)
			{
#ifdef _DEBUG
				printf("1) %g\n", matrix[i][j]);
#endif
				result_unknown += (double)matrix[i][j];
			}
#ifdef _DEBUG
			printf("2) (%g - %g) / %g\n", matrix[i][n], result, matrix[i][i]);
#endif

			if (matrix[i][i] == 0.0f)
			{
				/*
				 * if the coefficient for the pivot element is 0, that is,
				 * we skipped the line - we need to compare the left part of the matrix with the free coefficient,
				 * if they do not match, there is no solution for SoLE (System of linear equations)
				 */

				if (result_unknown == 0.0f)
				{
					/* *
					 * if it turned out that the sum on the left is 0,
					 * then it will not be possible to compare it with the free coefficient
					 * requires an adequacy check
					 */
#ifdef _DEBUG
					printf("comp %g > %g\n", fabsf(matrix[i][n]), machEps() * 6);
					printf("%f\n", matrix[i][n]);
#endif

					if (fabsf(matrix[i][n]) >= (machEps() * 6))	   // 6E-7)
					{
						result.many_solutions = false;
						break;
					}
				}
				else if (!AlmostEqual2sComplement((float)result_unknown, matrix[i][n], MAX_ULP))
				{
					result.many_solutions = false;
					break;
				}
				result_unknown = 0.0f;
			}
			else
			{
				/*
				 * if the coefficient for the pivot element is not equal to 0,
				 * we calculate the result
				 */
				result_unknown = ((double)matrix[i][n] - result_unknown);
				result_unknown /= (double)matrix[i][i];
			}

			/*
			 * let's write the calculated value of x immediately
			 * into the rest of the matrix,
			 * so as not to waste space for an array of answers
			 */
			for (size_t j = i; j--;)
			{
				matrix[j][i] = (float)((double)matrix[j][i] * result_unknown);
			}
#ifdef _DEBUG
			printf("i=%d result=%g\n", i, result);
			my_print_matrix("Matrix in research result", __LINE__, matrix, n, n + 1);
#endif
		}

#ifdef _DEBUG
		printf(result.many_solutions ? "many solutions" : "no solution");
#endif
	}
	else	// if SoLE (System of linear equations) has one solution
	{
#ifdef _DEBUG
		printf("norm\n");
#endif

		for (size_t i = n; i--;)
		{
			double result_unknown = matrix[i][n];
			for (size_t j = i + 1; j < n; j++)
			{
#ifdef _DEBUG
				printf("1) %g\n", matrix[i][j]);
#endif
				result_unknown -= (double)matrix[i][j];
			}
#ifdef _DEBUG
			printf("2) (%g + %g) / %g\n", matrix[i][n], matrix[i][n], matrix[i][i]);
#endif

			if (matrix[i][i] == 0.0)
			{
				/*
				 * if the pivot element is suddenly equal to 0,
				 * although the number of free variables is 0,
				 * then an unknown error has occurred
				 */
				my_log(true, error, __LINE__, "The pivot element is suddenly equal to 0\n");
				// return_code = ERROR_UNKNOWN;
				// goto close;
			}
			/*
			 * if the coefficient for the pivot element is not equal to 0,
			 * we calculate the result
			 */
			result_unknown /= (double)matrix[i][i];

			/*
			 * let's write the calculated value of x immediately
			 * into the rest of the matrix,
			 * so as not to waste space for an array of answers
			 */
			for (size_t j = i; j--;)
			{
				matrix[j][i] = (float)((double)matrix[j][i] * result_unknown);
			}

			/*
			 * let's write the calculated result to the last element of the line
			 * so as not to waste extra memory
			 */
			matrix[i][n] = (float)result_unknown;

#ifdef _DEBUG
			printf("i=%d result=%g\n", i, result);
			my_print_matrix("Matrix in research result", __LINE__, matrix, n, n + 1);
#endif
		}
	}

	return result;
}

Answer gauss_jordan_elimination(float **matrix, const size_t n)
{
	size_t n_independent_variable = forward_elimination(matrix, n);
	return back_substitution(matrix, n, n_independent_variable);
}