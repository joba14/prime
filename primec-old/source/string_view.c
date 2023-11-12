
/**
 * @file string_view.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#include <string_view.h>

#include <debug.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>

string_view_s string_view_from_parts(
	const char* const data,
	const uint64_t length)
{
	debug_assert(data != NULL);

	return (string_view_s)
	{
		.data = data,
		.length = length
	};
}

string_view_s string_view_from_cstring(
	const char* const cstring)
{
	debug_assert(cstring != NULL);
	const uint64_t length = (uint64_t)strlen(cstring);
	return string_view_from_parts(cstring, length);
}

bool string_view_equal_range(
	const string_view_s left,
	const string_view_s right,
	const uint64_t length)
{
	debug_assert(left.data != NULL);
	debug_assert(right.data != NULL);

	if (left.length < length || right.length < length)
	{
		return false;
	}

	return (memcmp(left.data, right.data, length) == 0);
}

bool string_view_equal(
	const string_view_s left,
	const string_view_s right)
{
	debug_assert(left.data != NULL);
	debug_assert(right.data != NULL);

	if (left.length != right.length)
	{
		return false;
	}

	return string_view_equal_range(left, right, left.length);
}

string_view_s string_view_trim_left(
	const string_view_s string_view,
	bool(*compare)(const string_view_s))
{
	debug_assert(string_view.data != NULL);
	debug_assert(compare != NULL);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while (index < string_view.length && compare(string_view_from_parts(string_view.data + index, string_view.length - index)))
	{
		++index;
	}

	return string_view_from_parts(string_view.data + index, string_view.length - index);
}

string_view_s string_view_trim_right(
	const string_view_s string_view,
	bool(*compare)(const string_view_s))
{
	debug_assert(string_view.data != NULL);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while (index < string_view.length && compare(string_view_from_parts(string_view.data + string_view.length - index - 1, string_view.length - index - 1)))
	{
		++index;
	}

	return string_view_from_parts(string_view.data, string_view.length - index);
}

string_view_s string_view_trim(
	const string_view_s string_view,
	bool(*compare)(const string_view_s))
{
	debug_assert(string_view.data != NULL);
	debug_assert(compare != NULL);
	return string_view_trim_left(string_view_trim_right(string_view, compare), compare);
}

bool string_view_left_split(
	const string_view_s string_view,
	bool(*compare)(const string_view_s),
	string_view_s* const left,
	string_view_s* const right)
{
	debug_assert_string_view(string_view);
	debug_assert(compare != NULL);
	debug_assert(left != NULL);
	debug_assert(right != NULL);

	if (string_view.length <= 0)
	{
		return false;
	}

	uint64_t index = 0;

	while (index < string_view.length && !compare(string_view_from_parts(string_view.data + index, string_view.length - index)))
	{
		++index;
	}

	*left = string_view_from_parts(string_view.data, index);
	*right = string_view_from_parts(string_view.data + index, string_view.length - index);
	return true;
}

bool string_view_right_split(
	const string_view_s string_view,
	bool(*compare)(const string_view_s),
	string_view_s* const left,
	string_view_s* const right)
{
	debug_assert_string_view(string_view);
	debug_assert(compare != NULL);
	debug_assert(left != NULL);
	debug_assert(right != NULL);

	if (string_view.length <= 0)
	{
		return false;
	}

	uint64_t index = 0;

	while (index < string_view.length && !compare(string_view_from_parts(string_view.data + string_view.length - index - 1, string_view.length - index - 1)))
	{
		++index;
	}

	*left = string_view_from_parts(string_view.data, index);
	*right = string_view_from_parts(string_view.data + index, string_view.length - index);
	return true;
}