
/**
 * @file prime_asserts.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#include <prime_common/prime_asserts.h>

#ifndef NDEBUG
#	include <stdlib.h>
#	include <stdio.h>

void prime_asserts_debug_assert(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line)
{
	if (!expression)
	{
		fprintf(stderr, "assertion failed: %s\n\tat %s:%llu\n",
			expression_string, file, (long long unsigned int)line);
		exit(-1);
	}
}
#endif
