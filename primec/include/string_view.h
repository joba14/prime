
/**
 * @file string_view.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __PRIMEC__INCLUDE__STRING_VIEW_H__
#define __PRIMEC__INCLUDE__STRING_VIEW_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
	const char* data;
	uint64_t length;
} string_view_t;

#define CSTRING_TO_STRING_VIEW(_cstring) { .data = _cstring, .length = ((uint64_t)(sizeof(_cstring) / sizeof(char)) - 1) }
#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_ARG(_string_view) ((signed int) (_string_view).length), (_string_view).data

string_view_t string_view_from_parts(
	const char* const data,
	const uint64_t length);

string_view_t string_view_from_cstring(
	const char* const cstring);

bool string_view_equal(
	const string_view_t left,
	const string_view_t right);

string_view_t string_view_trim_left(
	const string_view_t string_view);

string_view_t string_view_trim_right(
	const string_view_t string_view);

string_view_t string_view_trim(
	const string_view_t string_view);

#ifndef NDEBUG
#	include <debug.h>
#	include <stddef.h>
#	define debug_assert_string_view(_string_view)                              \
		do                                                                     \
		{                                                                      \
			debug_assert((_string_view).data != NULL);                         \
			debug_assert((_string_view).length > 0);                           \
		} while (0)
#else
#	define debug_assert_string_view(_string_view)
#endif

#endif
