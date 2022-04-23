#include "my_ptr_unchained.h"

void my_fclose(FILE **file)
{
	if (*file != NULL)
	{
		fclose(*file);
		*file = NULL;
	}
}

void my_free(void **ptr)
{
	if (*ptr != NULL)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

void my_free_matrix(void ***matrix, size_t rows)
{
	if (*matrix != NULL)
	{
		for (size_t i = 0; i < rows; i++)
		{
			if ((*matrix)[i] != NULL)
			{
				free((*matrix)[i]);
				(*matrix)[i] = NULL;
			}
		}
		free(*matrix);
		*matrix = NULL;
	}
}