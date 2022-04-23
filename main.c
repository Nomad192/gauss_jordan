#include "fp_comp.h"
#include "gauss_jordan_elimination_algorithm.h"
#include "my_logging.h"
#include "my_ptr_unchained.h"
#include "return_codes.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
	clear_log_file();
	my_log(false, __LINE__, verbose, "\nNEW PROGRAM\n");
	my_log(true, __LINE__, verbose, "Program \"%s\" starts with arguments:\n", argv[0]);
	for (int i = 1; i < argc; i++)
	{
		my_log(false, __LINE__, verbose, "%s\n", argv[i]);
	}
	my_log(false, __LINE__, verbose, "\n\n");

	if (argc != 3)
	{
		printf("HELP: "
			   "\t first argument ->  name_in_file "
			   "(with file extension REQUIRED and path if necessary)"
			   "\n\tsecond argument -> name_out_file "
			   "(with file extension REQUIRED and path if necessary)\n\n");
		my_print_err(
			critical_error,
			__LINE__,
			"I was given the wrong "
			"number of arguments: %d instead of 2\nEND\n\n",
			argc - 1);
		for (int i = 1; i < argc; i++)
		{
			my_print_err(critical_error, __LINE__, "arg %d = \"%s\"\n", i, argv[i]);
		}
		return ERROR_INVALID_PARAMETER;
	}

	///-------------------------------------------------------------------------------------------------------------///

	FILE *in = NULL;
	FILE *out = NULL;
	float **matrix = NULL;	  // array of arrays

	int return_code = ERROR_SUCCESS;	// the value that the program will return

	///-------------------------------------------------------------------------------------------------------------///
	size_t n = 0;	 // number of rows in the matrix (dimension of the matrix)

	in = fopen(argv[1], "r");
	if (in == NULL)
	{
		my_print_err(critical_error, __LINE__, "I can't open the input file - %s\nEND\n\n", argv[1]);
		return_code = ERROR_FILE_NOT_FOUND;
		goto close;
	}

	if (!fscanf(in, "%zu", &n))
	{
		my_print_err(critical_error, __LINE__, "There is no value \"number of lines\" in the in file - %s\nEND\n\n", argv[1]);
		return_code = ERROR_INVALID_DATA;
		goto close;
	}
	if (n == 0)
	{
		my_print_err(critical_error, __LINE__, "Value \"number of lines\" = 0 in the in input file - %s\nEND\n\n", argv[1]);
		return_code = ERROR_INVALID_DATA;
		goto close;
	}

	matrix = malloc(n * sizeof(float *));
	if (matrix == NULL)
	{
		my_print_err(critical_error, __LINE__, "I can't allocate memory for data matrix ptr lines - %zu float* elements\nEND\n\n", n);
		return_code = ERROR_OUTOFMEMORY;
		goto close;
	}

	for (size_t i = 0; i < n; i++)
	{
		matrix[i] = malloc((n + 1) * sizeof(float));
		if (matrix[i] == NULL)
		{
			n = i;
			my_print_err(
				critical_error,
				__LINE__,
				"I can't allocate memory for %zu line"
				" of data matrix - %zu float elements\nEND\n\n",
				i,
				n + 1);
			return_code = ERROR_OUTOFMEMORY;
			goto close;
		}
	}
	my_log(
		true,
		__LINE__,
		verbose,
		"Successfully allocated memory for data matrix"
		" and ptr array - %zy*%zy float + %zu float* elements\n\n",
		n,
		n + 1,
		n);

	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n + 1; j++)
		{
			if (!fscanf(in, "%f", &matrix[i][j]))
			{
				my_print_err(critical_error, __LINE__, "There is not enough input data to a complete system of size %zu * %zu\n\n", n, n + 1);
				return_code = ERROR_INVALID_DATA;
				goto close;
			}
		}
	}

	my_print_matrix("Matrix before", __LINE__, matrix, n, n + 1);	 // output only in the case of _DEBUG
	my_fclose(&in);													 // to check and to return NULL to the released file
	my_log(true, __LINE__, verbose, "Data from the %s (input file) was successfully read\n\n", argv[1]);
	///-------------------------------------------------------------------------------------------------------------///

	Answer answer = gauss_jordan_elimination(matrix, n);

	///-------------------------------------------------------------------------------------------------------------///

	out = fopen(argv[2], "w");
	if (out == NULL)
	{
		my_print_err(critical_error, __LINE__, "I can't create the output file - %s\n\n", argv[2]);
		return_code = ERROR_FILE_NOT_FOUND;
		goto close;
	}

	if (answer.only_solution)
	{
		my_log(true, __LINE__, info, "result - %zu lines\n\n", n);
		for (int i = 0; i < n; i++)
		{
			fprintf(out, "%g\n", matrix[i][n]);
		}
	}
	else
	{
		my_log(true, __LINE__, info, "result - %s\n\n", answer.many_solutions ? "many solutions" : "no solution");
		fprintf(out, answer.many_solutions ? "many solutions" : "no solution");
	}
	my_fclose(&out);

	my_log(true, __LINE__, verbose, "The response was successfully written to the %s (output file)\n\n", argv[2]);

	///-------------------------------------------------------------------------------------------------------------///

close:
	my_fclose(&in);
	my_fclose(&out);
#ifdef _DEBUG
	printf("\nmark del\n");
#endif
	my_free_matrix((void ***)&matrix, n);

	my_log(true, __LINE__, verbose, "The program ended successfully\nEND\n\n");

	return return_code;
}