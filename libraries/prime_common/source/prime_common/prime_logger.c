
/**
 * @file prime_logger.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-01
 */

#include <prime_common/prime_logger.h>

#include <prime_common/prime_asserts.h>

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#define ANSI_GRAY "\033[90m"
#define ANSI_RED "\033[91m"
#define ANSI_GREEN "\033[92m"
#define ANSI_YELLOW "\033[93m"
#define ANSI_RESET "\033[0m"

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args);

void prime_logger_info(
	const char* const format,
	...)
{
	prime_asserts_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stdout, ANSI_GREEN "info " ANSI_RESET, format, args);
	va_end(args);
}

void prime_logger_warn(
	const char* const format,
	...)
{
	prime_asserts_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stderr, ANSI_YELLOW "warn " ANSI_RESET, format, args);
	va_end(args);
}

void prime_logger_error(
	const char* const format,
	...)
{
	prime_asserts_assert(format);

	va_list args;
	va_start(args, format);
	log_with_tag(stderr, ANSI_RED "error" ANSI_RESET, format, args);
	va_end(args);
}

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args)
{
	prime_asserts_assert(stream);
	prime_asserts_assert(tag);
	prime_asserts_assert(format);

	#define logging_buffer_capacity 4096
	static char logging_buffer[logging_buffer_capacity];
	uint32_t length = (uint32_t)vsnprintf(
		logging_buffer, logging_buffer_capacity, format, args);
	logging_buffer[length++] = '\n';
	logging_buffer[length] = 0;
	#undef logging_buffer_capacity

	for (const char* buffer_iterator = logging_buffer,
		*line_iterator = logging_buffer; *buffer_iterator; ++buffer_iterator)
	{
		if (*buffer_iterator == '\n')
		{
			time_t now;
			time(&now);

			struct tm tm_info;
			char time_str[20];

			localtime_r(&now, &tm_info);
			strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

			fprintf(stream, "[" ANSI_GRAY "%s" ANSI_RESET "]: %s: %.*s\n",
				time_str, tag, (signed int)(buffer_iterator - line_iterator), line_iterator);

			line_iterator = buffer_iterator + 1;
		}
	}
}
