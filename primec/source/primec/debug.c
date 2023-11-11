
/**
 * @file debug.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-11
 */

#include <primec/debug.h>

#ifndef NDEBUG
#	include <stdlib.h>
#	include <stdio.h>

void primec_debug_assert_impl(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line)
{
	if (!expression)
	{
		(void)fprintf(stderr,
			"\033[91m" "\033[1m" "%s" "\033[0m" ": assertion `%s` failed at %s:%lu\n",
			"fatal", expression_string, file, line);
		exit(-1);
	}
}
#else
_Static_assert(1, ""); // NOTE: To prevent empty translaion unit error.
#endif
