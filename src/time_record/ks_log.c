#include "ks_log.h"
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

static FILE *logf = NULL;
static char ks_log_file_path[128]={0};
static int ks_default_log_level = KS_LOG_LEVEL_DEBUG;

void
ks_log_init(const char* log_path, int log_level)
{
	strncpy(ks_log_file_path, log_path, 128);
	ks_default_log_level = log_level;
}

void
ks_log_error(int level, const char *fmt, ...)
{
	if (level >= ks_default_log_level) {
		if (logf == NULL) {
			logf = fopen(ks_log_file_path, "a+");
		}
		char log_line[MAX_STRING_LEN];
		va_list args;
		va_start(args, fmt);
		vsnprintf(log_line, MAX_STRING_LEN, fmt, args);
		va_end(args);
		fputs(log_line, logf);
		fflush(logf);
	} else {
		return;
	}
}

void
ks_log_exit()
{
	if (logf != NULL) {
		fclose(logf);
	}
}
