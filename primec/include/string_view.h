
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

#ifndef __primec__include__string_view_h__
#define __primec__include__string_view_h__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief String view structure
 * 
 * It holds a const-pointer to a string or sequence of characters and a length.
 */
typedef struct
{
	const char* data;
	uint64_t length;
} string_view_t;

/**
 * @brief String view format for printf-like functions.
 */
#define sv_fmt "%.*s"

/**
 * @brief String view argument for printf-like functions.
 */
#define sv_arg(_string_view) ((signed int) (_string_view).length), (_string_view).data

/**
 * @brief Create string view from a const-pointer to a string or sequence of characters and its length.
 * 
 * @param data[in]   pointer to a string or sequence of characters
 * @param length[in] length of the string or sequence of characters
 * 
 * @return string_view_t
 */
string_view_t string_view_from_parts(
	const char* const data,
	const uint64_t length);

/**
 * @brief Create string view from a cstring.
 * 
 * @param cstring[in] pointer to a cstring
 * 
 * @return string_view_t
 */
string_view_t string_view_from_cstring(
	const char* const cstring);

/**
 * @brief Check if two string views are equal.
 * 
 * @param left[in]  left comparison string view
 * @param right[in] right comparison string view
 * 
 * @return bool
 */
bool string_view_equal(
	const string_view_t left,
	const string_view_t right);

/**
 * @brief Trim white space on the left side of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * 
 * @return string_view_t
 */
string_view_t string_view_trim_left(
	const string_view_t string_view);

/**
 * @brief Trim white space on the right side of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * 
 * @return string_view_t
 */
string_view_t string_view_trim_right(
	const string_view_t string_view);

/**
 * @brief Trim white space on both sides of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * 
 * @return string_view_t
 */
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
