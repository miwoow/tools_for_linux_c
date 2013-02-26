#ifndef KS_LOG_H
#define KS_LOG_H

#define KS_LOG_LEVEL_DEBUG 0
#define KS_LOG_LEVEL_WARNING 1
#define KS_LOG_LEVEL_ERROR  2

#define MAX_STRING_LEN 8192

void ks_log_init(const char* log_path, int log_level);
void ks_log_error(int level, const char *fmt, ...);
void ks_log_exit();

#endif
