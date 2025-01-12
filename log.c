#include "log.h"
#include "millis.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"
#include "gfx.h"
#include "colors.h"
#include "alloc.h"

typedef struct LOG
{
	u64 Time;
	LogLevel Level;
	char *Message;
	struct LOG *Next;
} Log;

static Log *_logs;

static const char *log_level_str(LogLevel ll)
{
	static const char *names[] =
	{
		"[ TRACE ]",
		"[ DEBUG ]",
		"[ INFO  ]",
		"[ WARN  ]",
		"[ ERROR ]",
		"[ FATAL ]"
	};

	assert(ll < ARRLEN(names));
	return names[ll];
}

static u32 log_level_color(LogLevel ll)
{
	static const u32 colors[] =
	{
		0x00FFFF,
		0x00FF00,
		0x0000FF,
		0xFFFF00,
		0xFF0000,
		0xFF00FF
	};

	assert(ll < ARRLEN(colors));
	return colors[ll];
}

static const char *format_time(u64 dt)
{
	static char buf[64];
	u64 dt_in_min = dt / 60000;
	u64 dt_in_sec = (dt - dt_in_min * 60000) / 1000;
	u64 dt_in_milisec = dt % 1000;

	snprintf(buf, sizeof(buf), "%02" PRIu64 ":%02" PRIu64 ":%03" PRIu64 "",
		dt_in_min, dt_in_sec, dt_in_milisec);
	return buf;
}

void log_add(u64 time, LogLevel level, const char *msg, int len)
{
	Log *log = _malloc(sizeof(Log));
	char *copy = _malloc(len + 1);
	memcpy(copy, msg, len + 1);
	log->Time = time;
	log->Level = level;
	log->Message = copy;
	log->Next = _logs;
	_logs = log;
}

void log_write(LogLevel l, int line, const char *file, const char *msg, ...)
{
	char format[256];
	va_list v;
	va_start(v, msg);
	vsnprintf(format, sizeof(format), msg, v);
	va_end(v);

	u64 dt = millis();
	printf("%s %s %s (%s:%d)\n",
		format_time(dt), log_level_str(l), format, file, line);

	char buf[512];
	int len = snprintf(buf, sizeof(buf), "%s (%s:%d)\n",
		format, file, line);
	log_add(dt, l, buf, len);
}

void log_render(i32 x, i32 y)
{
	Log *l = _logs;
	while(l)
	{
		gfx_string(x, y, COLOR_WHITE, format_time(l->Time));
		gfx_string(x + 100, y, log_level_color(l->Level), log_level_str(l->Level));
		gfx_string(x + 200, y, COLOR_WHITE, l->Message);
		l = l->Next;
		y -= 25;
	}
}
