
/**
 * @file main.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __PRIMEC__INCLUDE__MAIN_H__
#define __PRIMEC__INCLUDE__MAIN_H__

#include <string_view.h>

typedef enum
{
	cli_option_type_help = 0,
	cli_option_type_with_outputs,
	cli_option_types_count
} cli_option_type_t;

typedef struct
{
	string_view_t short_name;
	string_view_t long_name;
	string_view_t description;
} cli_option_descriptor_t;

signed int main(
	const signed int argc,
	const char** const argv);

#endif
