
/**
 * @file options.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#include <options.h>

#include <debug.h>
#include <logger.h>

#include <stddef.h>
#include <stdlib.h>

implement_linked_list_type(options, string_view_s);

typedef enum
{
	cli_option_type_help = 0,
	cli_option_type_with_outputs,
	cli_option_types_count
} option_type_e;

typedef struct
{
	option_type_e type;
	string_view_s short_name;
	string_view_s long_name;
	string_view_s description;
} option_descriptor_s;

static string_view_s g_program = {0};
#define cstr2sv(_cstring) { .data = _cstring, .length = ((uint64_t)(sizeof(_cstring) / sizeof(char)) - 1) }
static option_descriptor_s g_option_descriptors[cli_option_types_count] =
{
	[cli_option_type_help] =
	{
		.type = cli_option_type_help,
		.short_name  = cstr2sv("-h"),
		.long_name   = cstr2sv("--help"),
		.description = cstr2sv("print this options_usage/help message.")
	},
	[cli_option_type_with_outputs] =
	{
		.type = cli_option_type_with_outputs,
		.short_name  = cstr2sv("-wo"),
		.long_name   = cstr2sv("--with-outputs"),
		.description = cstr2sv("treat command line arguments in pairs (input and output path pairs).")
	}
};

void options_parse(
	options_s* const options,
	const uint64_t argc,
	const char** const argv,
	bool* const with_outputs)
{
	debug_assert(options != NULL);
	debug_assert(argc > 0);
	debug_assert(argv != NULL);
	debug_assert(with_outputs != NULL);

	g_program = string_view_from_cstring(argv[0]);
	const uint64_t argc_count_without_program_name = (uint64_t)(argc - 1);

	if (argc_count_without_program_name <= 0)
	{
		logger_error("no command line arguments were provided!");
		options_usage();
		exit(-1);
	}

	for (uint64_t index = 0; index < argc_count_without_program_name; ++index)
	{
		options_push_tail(options,
			string_view_from_cstring(argv[index + 1])
		);
	}

	string_view_s first_argument = options->head->data;

	if (string_view_equal(first_argument, g_option_descriptors[cli_option_type_help].short_name)
	 || string_view_equal(first_argument, g_option_descriptors[cli_option_type_help].long_name))
	{
		options_usage();
		exit(0);
	}
	else if (string_view_equal(first_argument, g_option_descriptors[cli_option_type_with_outputs].short_name)
		  || string_view_equal(first_argument, g_option_descriptors[cli_option_type_with_outputs].long_name))
	{
		*with_outputs = true;

		if ((options->count - 1) <= 0)
		{
			logger_error("insufficient paths provided!");
			options_usage();
			exit(-1);
		}

		if ((options->count - 1) % 2 != 0)
		{
			logger_error("odd amount of command line argumets provided with option `" sv_fmt "`!", sv_arg(first_argument));
			options_usage();
			exit(-1);
		}

		(void)options_pop_head(options);
	}
}

void options_usage(
	void)
{
	debug_assert_string_view(g_program);

	logger_info("options_usage: " sv_fmt " [options] <paths...>", sv_arg(g_program));
	logger_info(" ");

	logger_info("description:");
	logger_info("this executable creates intermediate representation(IR) files from prime source code files.");
	logger_info(" ");

	logger_info("options:");
	for (uint64_t index = 0; index < cli_option_types_count; ++index)
	{
		const option_descriptor_s* const option_descriptor =
			(const option_descriptor_s* const)&g_option_descriptors[index];
		debug_assert(option_descriptor != NULL);

		debug_assert_string_view(option_descriptor->short_name);
		debug_assert_string_view(option_descriptor->long_name);
		debug_assert_string_view(option_descriptor->description);

		logger_info("    " sv_fmt ", " sv_fmt "\n        " sv_fmt,
			sv_arg(option_descriptor->short_name),
			sv_arg(option_descriptor->long_name),
			sv_arg(option_descriptor->description)
		);
	}
	logger_info(" ");

	logger_info("copyright notice:");
	logger_info("this executable is part of the \"Prime\" project and is distributed under \"Prime GPLv1\" license.");
	logger_info(" ");
}
