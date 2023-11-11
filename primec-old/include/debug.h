
/**
 * @file debug.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __primec__include__debug_h__
#define __primec__include__debug_h__

#ifndef NDEBUG
#	include <stdbool.h>
#	include <stdint.h>

void debug_assert_impl(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line);

#	define debug_assert(_expression)                                           \
		debug_assert_impl(                                                     \
			_expression,                                                       \
			#_expression,                                                      \
			(const char*)__FILE__,                                             \
			(uint64_t)__LINE__                                                 \
		)
#else
#	define debug_assert(_expression)
#endif

#endif
