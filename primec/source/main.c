
/**
 * @file main.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#include <main.h>

#include <debug.h>
#include <logger.h>

#include <string_view.h>
#include <heap_buffer.h>

#include <cli_parser.h>
#include <tokenizer.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

static cli_parser_s g_cli_parser;
static string_view_s g_with_outputs_flag;

static cli_arguments_list_s parse_command_line(
	const uint64_t argc,
	const char** const argv);

signed int main(
	const signed int argc,
	const char** const argv)
{
	// TODO: remove:
	printf(sv_fmt "+", sv_arg(string_view_from_cstring("")));

	cli_arguments_list_s arguments = parse_command_line(
		(uint64_t)argc, argv
	);

	const bool with_outputs = string_view_equal(
		g_with_outputs_flag, string_view_from_cstring("true")
	);

	if (with_outputs)
	{
		if (arguments.count <= 0)
		{
			logger_error("insufficient paths provided!");
			cli_parser_usage(&g_cli_parser);
			exit(-1);
		}

		if (arguments.count % 2 != 0)
		{
			logger_error("odd amount of command line argumets provided with option [ -wo | --with-outputs ]!");
			cli_parser_usage(&g_cli_parser);
			exit(-1);
		}
	}

	for (uint64_t index = 0; index < arguments.count; ++index)
	{
		const string_view_s input_file_path =
			*cli_arguments_list_at(&arguments, index);

		tokenizer_s tokenizer = tokenizer_from_parts(input_file_path);
		tokens_list_s tokens = tokenizer_process_file(&tokenizer);
		(void)tokens;
		// TODO: tokens!

		string_view_s output_file_path = input_file_path;

		if (with_outputs)
		{
			output_file_path = *cli_arguments_list_at(&arguments, ++index);
		}

		logger_info(sv_fmt " -> " sv_fmt,
			sv_arg(input_file_path), sv_arg(output_file_path)
		);
	}

	return 0;
}

static cli_arguments_list_s parse_command_line(
	const uint64_t argc,
	const char** const argv)
{
	debug_assert(argc > 0);
	debug_assert(argv != NULL);

	g_cli_parser = cli_parser_from_parts(
		string_view_from_cstring(argv[0]),
		string_view_from_cstring("this executable creates intermediate representation (IR) files from prime source code files."),
		string_view_from_cstring("this executable is part of the \"prime\" project and is distributed under \"prime gplv1\" license."),
		(uint64_t)(argc - 1)
	);

	g_with_outputs_flag = string_view_from_cstring("false");
	cli_parser_add_option(&g_cli_parser,
		cli_option_without_value,
		string_view_from_cstring("-wo"),
		string_view_from_cstring("--with-outputs"),
		string_view_from_cstring("treat command line arguments in pairs (input and output path pairs)."),
		&g_with_outputs_flag
	);

	cli_arguments_list_s arguments = cli_parser_parse(
		&g_cli_parser, (uint64_t)argc, argv
	);

	return arguments;
}
