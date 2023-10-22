
/**
 * @file logger.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#include <logger.h>

#include <debug.h>

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#define ANSI_GRAY   "\033[90m"
#define ANSI_RED    "\033[91m"
#define ANSI_GREEN  "\033[92m"
#define ANSI_YELLOW "\033[93m"
#define ANSI_RESET  "\033[0m"

#define TAG_INFO  "info "
#define TAG_WARN  "warn "
#define TAG_ERROR "error"

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args);

void logger_info(
	const char* const format,
	...)
{
	debug_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stdout, ANSI_GREEN TAG_INFO ANSI_RESET, format, args);
	va_end(args);
}

void logger_warn(
	const char* const format,
	...)
{
	debug_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stderr, ANSI_YELLOW TAG_WARN ANSI_RESET, format, args);
	va_end(args);
}

void logger_error(
	const char* const format,
	...)
{
	debug_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stderr, ANSI_RED TAG_ERROR ANSI_RESET, format, args);
	va_end(args);
}

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args)
{
	debug_assert(stream);
	debug_assert(tag);
	debug_assert(format);

	#define logging_buffer_capacity 4096
	static char logging_buffer[logging_buffer_capacity];
	uint64_t length = (uint64_t)vsnprintf(
		logging_buffer, logging_buffer_capacity, format, args);
	logging_buffer[length++] = '\n';
	logging_buffer[length] = 0;
	#undef logging_buffer_capacity

	for (const char* buffer_iterator = logging_buffer, *line_iterator = logging_buffer; *buffer_iterator; ++buffer_iterator)
	{
		const bool isSpace = *buffer_iterator == ' ';
		const bool isNewline = *buffer_iterator == '\n';
		const bool isOver80Symbols = (uint64_t)(buffer_iterator - line_iterator) >= 80;

		if ((isOver80Symbols && isSpace) || isNewline)
		{
			time_t now;
			(void)time(&now);

			struct tm tm_info;
			char time_str[20];

			(void)localtime_r(&now, &tm_info);
			(void)strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

			(void)fprintf(stream, "[" ANSI_GRAY "%s" ANSI_RESET "]: %s: %.*s\n",
				time_str, tag, (signed int)(buffer_iterator - line_iterator), line_iterator
			);

			line_iterator = buffer_iterator + 1;
		}
	}
}
