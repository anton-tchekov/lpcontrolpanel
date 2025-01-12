#ifndef __LOG_H__
#define __LOG_H__

#include "types.h"

typedef enum
{
	TRACE,
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL
} LogLevel;

#define log_trace(...) log_write(TRACE, __LINE__, __FILE__, __VA_ARGS__)
#define log_debug(...) log_write(DEBUG, __LINE__, __FILE__, __VA_ARGS__)
#define log_info(...)  log_write(INFO,  __LINE__, __FILE__, __VA_ARGS__)
#define log_warn(...)  log_write(WARN,  __LINE__, __FILE__, __VA_ARGS__)
#define log_error(...) log_write(ERROR, __LINE__, __FILE__, __VA_ARGS__)
#define log_fatal(...) log_write(FATAL, __LINE__, __FILE__, __VA_ARGS__)

void log_write(LogLevel l, int line, const char *file, const char *msg, ...);
void log_render(i32 x, i32 y);

#endif
