
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

#ifndef __primec__include__main_h__
#define __primec__include__main_h__

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

/**
 * @brief Entry point to the program.
 * 
 * @warning Should not be called anywhere!
 * 
 * @param argc arguments count
 * @param argv pointer to arguments sequence
 * 
 * @return signed int
 */
signed int main(
	const signed int argc,
	const char** const argv);

#endif
