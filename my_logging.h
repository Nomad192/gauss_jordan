#ifndef MY_LOGGING_H
#define MY_LOGGING_H

#define LOGS_NAME "c_1_Nomad192_logs.txt"
//#define LOG_ON verbose
//#define CLEAR_FILE_LOG
//#define _DEBUG

#include "return_codes.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum type_logging_t
{
	critical_error,
	error,
	warning,
	info,
	verbose
} Type_Logging;

/// Does not output an error when not clearing the file
void clear_log_file();

int my_log(bool w_time, size_t line, Type_Logging level, const char *str_log, ...);

int vmy_log(bool w_time, size_t line, Type_Logging level, const char *str_log, va_list args);

int my_print_err(Type_Logging level, size_t line, const char *str_err, ...);

void my_print_matrix(const char *str, int line, float **matrix, size_t row, size_t col);

#endif	  // MY_LOGGING_H
