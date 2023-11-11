
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

/**
 * @brief String view structure.
 * 
 * It holds a const-pointer to a string or sequence of characters and a length.
 */
typedef struct
{
	const char* data;
	uint64_t length;
} string_view_s;

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
 * @return string_view_s
 */
string_view_s string_view_from_parts(
	const char* const data,
	const uint64_t length);

/**
 * @brief Create string view from a cstring.
 * 
 * @param cstring[in] pointer to a cstring
 * 
 * @return string_view_s
 */
string_view_s string_view_from_cstring(
	const char* const cstring);

/**
 * @brief Check if two string views are equal for provided length (range).
 * 
 * @param left[in]   left comparison string view
 * @param right[in]  right comparison string view
 * @param length[in] length of a range to check for equality
 * 
 * @return bool
 */
bool string_view_equal_range(
	const string_view_s left,
	const string_view_s right,
	const uint64_t length);

/**
 * @brief Check if two string views are equal.
 * 
 * @param left[in]  left comparison string view
 * @param right[in] right comparison string view
 * 
 * @return bool
 */
bool string_view_equal(
	const string_view_s left,
	const string_view_s right);

/**
 * @brief Trim a sequence of characters decided by 'compare' function on the left side of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * @param compare[in]     compare function for trimming characters selection
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_left(
	const string_view_s string_view,
	bool(*compare)(const string_view_s));

/**
 * @brief Trim a sequence of characters decided by 'compare' function on the right side of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * @param compare[in]     compare function for trimming characters selection
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_right(
	const string_view_s string_view,
	bool(*compare)(const string_view_s));

/**
 * @brief Trim a sequence of characters decided by 'compare' function on both sides of the string view.
 * 
 * @param string_view[in] string view to be trimmed
 * @param compare[in]     compare function for trimming characters selection
 * 
 * @return string_view_s
 */
string_view_s string_view_trim(
	const string_view_s string_view,
	bool(*compare)(const string_view_s));

/**
 * @brief Split string view into two by a provided compare function result from the left side.
 * 
 * @param string_view[in] string view to split
 * @param compare[in]     compare function for splitting characters selection
 * @param left[out]       left side string view after split
 * @param right[out]      right side string view after split
 * 
 * @return bool
 */
bool string_view_left_split(
	const string_view_s string_view,
	bool(*compare)(const string_view_s),
	string_view_s* const left,
	string_view_s* const right);

/**
 * @brief Split string view into two by a provided compare function result from the right side.
 * 
 * @param string_view[in] string view to split
 * @param compare[in]     compare function for splitting characters selection
 * @param left[out]       left side string view after split
 * @param right[out]      right side string view after split
 * 
 * @return bool
 */
bool string_view_right_split(
	const string_view_s string_view,
	bool(*compare)(const string_view_s),
	string_view_s* const left,
	string_view_s* const right);

#ifndef NDEBUG
#	define debug_assert_string_view(_string_view)                              \
		do                                                                     \
		{                                                                      \
			debug_assert((_string_view).data != NULL);                         \
		} while (0)
#else
#	define debug_assert_string_view(_string_view)
#endif

#endif
