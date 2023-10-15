
/**
 * @file prime_asserts.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#ifndef __PRIME_COMMON__INCLUDE__PRIME_COMMON__PRIME_ASSERTS_H__
#define __PRIME_COMMON__INCLUDE__PRIME_COMMON__PRIME_ASSERTS_H__

#ifndef NDEBUG
#	include <stdbool.h>
#	include <stdint.h>

void prime_asserts_debug_assert(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line);

#	define prime_asserts_assert(_expression) \
		prime_asserts_debug_assert(_expression, #_expression, (const char*)__FILE__, (uint64_t)__LINE__)
#else
#	define prime_asserts_assert(_expression)
#endif

#endif
