
/**
 * @file logger.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-11
 */

#include <primec/logger.h>

#include <primec/debug.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#define ANSI_GRAY   "\033[90m"
#define ANSI_RED    "\033[91m"
#define ANSI_GREEN  "\033[92m"
#define ANSI_YELLOW "\033[93m"
#define ANSI_BROWN  "\033[33m"
#define ANSI_RESET  "\033[0m"

#define TAG_DEBUG "debug"
#define TAG_INFO  "info"
#define TAG_WARN  "warn"
#define TAG_ERROR "error"
#define TAG_PANIC "panic"

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args);

void primec_logger_log(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, NULL, format, args);
	va_end(args);
}

void primec_logger_debug(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, ANSI_BROWN TAG_DEBUG ANSI_RESET, format, args);
	va_end(args);
}

void primec_logger_info(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, ANSI_GREEN TAG_INFO ANSI_RESET, format, args);
	va_end(args);
}

void primec_logger_warn(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ANSI_YELLOW TAG_WARN ANSI_RESET, format, args);
	va_end(args);
}

void primec_logger_error(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ANSI_RED TAG_ERROR ANSI_RESET, format, args);
	va_end(args);
}

void primec_logger_panic(
	const char* const format,
	...)
{
	primec_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ANSI_RED TAG_PANIC ANSI_RESET, format, args);
	va_end(args);
	exit(-1);
}

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args)
{
	primec_debug_assert(stream != NULL);
	primec_debug_assert(format != NULL);

	#define logging_buffer_capacity 2048
	static char logging_buffer[logging_buffer_capacity + 1];
	uint64_t length = (uint64_t)vsnprintf(
		logging_buffer, logging_buffer_capacity, format, args);
	logging_buffer[length++] = '\n';
	logging_buffer[length] = 0;
	#undef logging_buffer_capacity

	if (NULL == tag)
	{
		(void)fprintf(stream, "%.*s", (signed int)length, logging_buffer);
	}
	else
	{
		(void)fprintf(stream, "%s: %.*s", tag, (signed int)length, logging_buffer);
	}
}
