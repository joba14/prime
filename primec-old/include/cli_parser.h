
/**
 * @file cli_parser.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#ifndef __primec__include__cli_parser_h__
#define __primec__include__cli_parser_h__

#include <string_view.h>
#include <heap_buffer.h>

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	cli_option_with_value = 0,
	cli_option_without_value
} cli_option_e;

typedef struct
{
	cli_option_e type;
	string_view_s short_name;
	string_view_s long_name;
	string_view_s description;
	string_view_s original_name;
	string_view_s* bind;
	bool parsed;
} cli_option_s;

define_heap_buffer_type(cli_options_list, cli_option_s);
define_heap_buffer_type(cli_arguments_list, string_view_s);

typedef struct
{
	string_view_s program;
	string_view_s description;
	string_view_s epilog;
	cli_options_list_s options;
} cli_parser_s;

cli_parser_s cli_parser_from_parts(
	const string_view_s program,
	const string_view_s description,
	const string_view_s epilog,
	const uint64_t capacity);

void cli_parser_add_option(
	cli_parser_s* const cli_parser,
	const cli_option_e type,
	const string_view_s short_name,
	const string_view_s long_name,
	const string_view_s description,
	string_view_s* const bind);

cli_arguments_list_s cli_parser_parse(
	cli_parser_s* const cli_parser,
	const uint64_t argc,
	const char** const argv);

void cli_parser_usage(
	const cli_parser_s* const cli_parser);

#endif
