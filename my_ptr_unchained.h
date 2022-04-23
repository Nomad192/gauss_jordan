#ifndef MY_PTR_UNCHAINED_H
#define MY_PTR_UNCHAINED_H

#include <stdio.h>
#include <stdlib.h>

void my_fclose(FILE **file);

void my_free(void **ptr);

void my_free_matrix(void ***matrix, size_t rows);

#endif	  // MY_PTR_UNCHAINED_H
