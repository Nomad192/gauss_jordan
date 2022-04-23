#include "my_logging.h"

void clear_log_file()
{
#ifdef CLEAR_FILE_LOG
	remove(LOGS_NAME);
#endif
}

int my_log(const bool w_time, const size_t line, const Type_Logging level, const char *str_log, ...)
{
#ifdef LOG_ON
	if (LOG_ON >= level)
	{
		FILE *out_logs = fopen(LOGS_NAME, "a");
		if (!out_logs)
		{
			printf("I can't create a file for logging errors");
			return ERROR_NOT_FOUND;
		}
		if (w_time)
		{
			time_t current_time;
			current_time = time(NULL);
			fprintf(out_logs, "%s", ctime(&current_time));
		}
		va_list args;
		va_start(args, str_log);
		fprintf(out_logs, "in line - %zu\n", line);
		vfprintf(out_logs, str_log, args);
		va_end(args);
		fclose(out_logs);
	}
#endif
	return 0;
}

int vmy_log(const bool w_time, const size_t line, const Type_Logging level, const char *str_log, va_list args)
{
#ifdef LOG_ON
	if (LOG_ON >= level)
	{
		FILE *out_logs = fopen(LOGS_NAME, "a");
		if (!out_logs)
		{
			printf("I can't create a file for logging errors");
			return ERROR_NOT_FOUND;
		}
		if (w_time)
		{
			time_t current_time;
			current_time = time(NULL);
			fprintf(out_logs, "%s ", ctime(&current_time));
		}
		fprintf(out_logs, "in line - %zu\n", line);
		vfprintf(out_logs, str_log, args);

		fclose(out_logs);
	}
#endif
	return 0;
}

int my_print_err(const Type_Logging level, const size_t line, const char *str_err, ...)
{
	int result;
	va_list args;
	va_start(args, str_err);
	vprintf(str_err, args);
	result = vmy_log(true, line, level, str_err, args);
	va_end(args);
	return result;
}

void my_print_matrix(const char *str, const int line, float **matrix, size_t row, size_t col)
{
#ifdef _DEBUG
	printf("\n%s, in line - %zu:\n", str, line);
	for (size_t i = 0; i < row; i++)
	{
		for (size_t j = 0; j < col; j++)
		{
			printf("%f ", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
#endif
}