
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

#include <cli_parser.h>

#include <debug.h>
#include <logger.h>

#include <stddef.h>
#include <stdlib.h>

implement_heap_buffer_type(cli_options_list, cli_option_s);
implement_heap_buffer_type(cli_arguments_list, string_view_s);

cli_parser_s cli_parser_from_parts(
	const string_view_s program,
	const string_view_s description,
	const string_view_s epilog,
	const uint64_t capacity)
{
	debug_assert_string_view(program);
	debug_assert_string_view(description);
	debug_assert_string_view(epilog);

	return (cli_parser_s)
	{
		.program = program,
		.description = description,
		.epilog = epilog,
		.options = cli_options_list_from_parts(capacity + 1)
	};
}

void cli_parser_add_option(
	cli_parser_s* const cli_parser,
	const cli_option_e type,
	const string_view_s short_name,
	const string_view_s long_name,
	const string_view_s description,
	string_view_s* const bind)
{
	debug_assert(cli_parser != NULL);
	debug_assert_string_view(short_name);
	debug_assert_string_view(long_name);
	debug_assert_string_view(description);
	debug_assert(bind != NULL);

	cli_options_list_append(
		&cli_parser->options,
		(cli_option_s)
		{
			.type = type,
			.short_name = short_name,
			.long_name = long_name,
			.description = description,
			.original_name = long_name,
			.bind = bind,
			.parsed = false
		}
	);
}

cli_arguments_list_s cli_parser_parse(
	cli_parser_s* const cli_parser,
	const uint64_t argc,
	const char** const argv)
{
	debug_assert(cli_parser != NULL);
	debug_assert(argc > 0);
	debug_assert(argv != NULL);

	const uint64_t argc_count_without_program_name = (uint64_t)(argc - 1);
	cli_arguments_list_s arguments = cli_arguments_list_from_parts(argc_count_without_program_name + 1);

	if (argc_count_without_program_name <= 0)
	{
		logger_error("no command line arguments were provided!");
		cli_parser_usage(cli_parser);
		exit(-1);
	}

	for (const char** argv_iterator = argv + 1; *argv_iterator != NULL; ++argv_iterator)
	{
		const string_view_s argv_argument = string_view_from_cstring(*argv_iterator);

		if (string_view_equal(argv_argument, string_view_from_cstring("-h"))
		 || string_view_equal(argv_argument, string_view_from_cstring("--help")))
		{
			cli_parser_usage(cli_parser);
			exit(0);
		}

		cli_option_s* cli_option = NULL;

		for (uint64_t index = 0; index < cli_parser->options.count; ++index)
		{
			const cli_option_s* const temp_cli_option =
				cli_options_list_at(&cli_parser->options, index);
			debug_assert(temp_cli_option != NULL);

			if (string_view_equal(argv_argument, temp_cli_option->short_name)
			 || string_view_equal(argv_argument, temp_cli_option->long_name))
			{
				cli_option = (cli_option_s*)temp_cli_option;
				cli_option->original_name = argv_argument;
				break;
			}
		}

		if (NULL == cli_option)
		{
			cli_arguments_list_append(&arguments, argv_argument);
			continue;
		}

		if (cli_option_with_value == cli_option->type)
		{
			if (NULL == *(argv_iterator + 1))
			{
				logger_error("missing command line value for argument `" sv_fmt "`!", sv_arg(cli_option->original_name));
				cli_parser_usage(cli_parser);
				exit(-1);
			}

			*cli_option->bind = string_view_from_cstring(*(++argv_iterator));
		}
		else // cli_option_without_value == cli_option->type
		{
			*cli_option->bind = string_view_from_cstring("true");
		}

		cli_option->parsed = true;
	}

	cli_arguments_list_shrink(&arguments);
	return arguments;
}

void cli_parser_usage(
	const cli_parser_s* const cli_parser)
{
	debug_assert(cli_parser != NULL);
	debug_assert_string_view(cli_parser->program);
	debug_assert_string_view(cli_parser->description);
	debug_assert_string_view(cli_parser->epilog);
	debug_assert_heap_buffer(cli_parser->options);

	logger_info("options_usage: " sv_fmt " [options] <paths...>", sv_arg(cli_parser->program));
	logger_info(" ");

	logger_info("description:");
	logger_info(sv_fmt, sv_arg(cli_parser->description));
	logger_info(" ");

	logger_info("options:");
	for (uint64_t index = 0; index < cli_parser->options.count; ++index)
	{
		const cli_option_s* const cli_option =
			cli_options_list_at((cli_options_list_s* const)&cli_parser->options, index);
		debug_assert(cli_option != NULL);

		debug_assert_string_view(cli_option->short_name);
		debug_assert_string_view(cli_option->long_name);
		debug_assert_string_view(cli_option->description);

		logger_info("    " sv_fmt ", " sv_fmt "\n        " sv_fmt,
			sv_arg(cli_option->short_name),
			sv_arg(cli_option->long_name),
			sv_arg(cli_option->description)
		);
	}
	logger_info(" ");

	logger_info("epilog:");
	logger_info(sv_fmt, sv_arg(cli_parser->epilog));
	logger_info(" ");
}
