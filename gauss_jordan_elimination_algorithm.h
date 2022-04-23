#ifndef GAUSS_JORDAN_ELIMINATION_H
#define GAUSS_JORDAN_ELIMINATION_H

#include "fp_comp.h"
#include "my_logging.h"

#include <math.h>
#include <stdlib.h>

#define MAX_ULP 100000

void swap(size_t *a, size_t *b);
size_t pre_pivoting(float **matrix, size_t n);

typedef struct element_t
{
	double left;
	double right;
	size_t line;
} Element;

float smart_sub(Element *element, float current, float subtracted, float piv, float multiplied);
void pivoting(float **matrix, Element *maxi, size_t n, size_t prev_piv_c, size_t n_unknown);
size_t pivoting_and_calculating_column(float **matrix, size_t n, size_t prev_piv_c, size_t n_unknown);
void calculating_remaining_matrix(float **matrix, size_t n, size_t prev_piv_c, size_t n_unknown);
size_t forward_elimination(float **matrix, size_t n);

typedef struct answer_t
{
	bool only_solution;
	bool many_solutions;
} Answer;

Answer back_substitution(float **matrix, size_t n, size_t n_independent_variable);
Answer gauss_jordan_elimination(float **matrix, size_t n);

#endif	  // GAUSS_JORDAN_ELIMINATION_H
