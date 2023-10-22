
/**
 * @file options.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#ifndef __primec__include__options_h__
#define __primec__include__options_h__

#include <string_view.h>
#include <linked_list.h>

define_linked_list_type(options, string_view_s);

void options_parse(
	options_s* const options,
	const uint64_t argc,
	const char** const argv,
	bool* const with_outputs);

void options_usage(
	void);

#endif
