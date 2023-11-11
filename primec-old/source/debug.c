
/**
 * @file debug.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#include <debug.h>

#ifndef NDEBUG
#	include <stdlib.h>
#	include <stdio.h>
#	include <time.h>

void debug_assert_impl(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line)
{
	if (!expression)
	{
		time_t now = {0};
		(void)time(&now);

		struct tm tm_info = {0};
		char time_str[20] = {0};

		(void)localtime_r(&now, &tm_info);
		(void)strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

		(void)fprintf(stderr,
			"[" "\033[90m" "%s" "\033[0m" "]: " "\033[91m" "\033[1m" "%s" "\033[0m" ": assertion `%s` failed at %s:%lu\n",
			time_str, "fatal", expression_string, file, line);
		exit(-1);
	}
}
#endif
