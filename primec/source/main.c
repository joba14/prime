
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
#include <ring_buffer.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define arguments_buffer_capacity 128
define_ring_buffer_type(arguments_buffer, string_view_t, arguments_buffer_capacity)

static cli_option_descriptor_t g_cli_option_descriptors[cli_option_types_count] =
{
	[cli_option_type_help] =
	{
		.short_name  = CSTRING_TO_STRING_VIEW("-h"),
		.long_name   = CSTRING_TO_STRING_VIEW("--help"),
		.description = CSTRING_TO_STRING_VIEW("print this usage/help message.")
	},
	[cli_option_type_with_outputs] =
	{
		.short_name  = CSTRING_TO_STRING_VIEW("-wo"),
		.long_name   = CSTRING_TO_STRING_VIEW("--with-outputs"),
		.description = CSTRING_TO_STRING_VIEW("treat command line arguments in pairs (input and output paths).")
	}
};

static void usage(
	const string_view_t program);

signed int main(
	const signed int argc,
	const char** const argv)
{
	const string_view_t program = string_view_from_cstring(argv[0]);

	arguments_buffer_t arguments;
	arguments_buffer_reset(&arguments);

	for (uint64_t index = 0; index < (uint64_t)(argc - 1); ++index)
	{
		(void)arguments_buffer_give(&arguments, string_view_from_cstring(argv[index + 1]));
	}

	if (arguments.count <= 0)
	{
		logger_error("no command line arguments were provided!");
		usage(program);
		exit(-1);
	}

	bool with_outputs = false;
	string_view_t first_argument;
	(void)arguments_buffer_peek(&arguments, &first_argument);

	if (string_view_equal(first_argument, g_cli_option_descriptors[cli_option_type_help].short_name)
	 || string_view_equal(first_argument, g_cli_option_descriptors[cli_option_type_help].long_name))
	{
		usage(program);
		exit(0);
	}
	else if (string_view_equal(first_argument, g_cli_option_descriptors[cli_option_type_with_outputs].short_name)
		  || string_view_equal(first_argument, g_cli_option_descriptors[cli_option_type_with_outputs].long_name))
	{
		with_outputs = true;

		if ((arguments.count - 1) <= 0)
		{
			logger_error("insufficient paths provided!");
			usage(program);
			exit(-1);
		}

		if ((arguments.count - 1) % 2 != 0)
		{
			logger_error("odd amount of command line argumets provided with option `" STRING_VIEW_FMT "`!",
				STRING_VIEW_ARG(first_argument));
			usage(program);
			exit(-1);
		}

		string_view_t dummy;
		(void)arguments_buffer_take(&arguments, &dummy);
	}

	string_view_t input_argument = {0};
	while (arguments_buffer_take(&arguments, &input_argument))
	{
		const string_view_t input_file_path = input_argument;
		string_view_t output_file_path = input_argument;

		if (with_outputs)
		{
			string_view_t output_argument = {0};
			(void)arguments_buffer_take(&arguments, &output_argument);
			output_file_path = output_argument;
		}

		logger_info(STRING_VIEW_FMT " -> " STRING_VIEW_FMT,
			STRING_VIEW_ARG(input_file_path), STRING_VIEW_ARG(output_file_path)
		);
	}

	return 0;
}

static void usage(
	const string_view_t program)
{
	debug_assert_string_view(program);

	logger_info("usage: " STRING_VIEW_FMT " [options] <paths...>", STRING_VIEW_ARG(program));
	logger_info(" ");

	logger_info("description:");
	logger_info("this executable creates intermediate representation(IR) files from prime source code files.");
	logger_info(" ");

	logger_info("options:");
	for (uint64_t index = 0; index < cli_option_types_count; ++index)
	{
		const cli_option_descriptor_t* const cli_option_descriptor =
			(const cli_option_descriptor_t* const)&g_cli_option_descriptors[index];
		debug_assert(cli_option_descriptor != NULL);

		debug_assert_string_view(cli_option_descriptor->short_name);
		debug_assert_string_view(cli_option_descriptor->long_name);
		debug_assert_string_view(cli_option_descriptor->description);

		logger_info("    " STRING_VIEW_FMT ", " STRING_VIEW_FMT "\n        " STRING_VIEW_FMT,
			STRING_VIEW_ARG(cli_option_descriptor->short_name),
			STRING_VIEW_ARG(cli_option_descriptor->long_name),
			STRING_VIEW_ARG(cli_option_descriptor->description)
		);
	}
	logger_info(" ");

	logger_info("copyright notice:");
	logger_info("this executable is part of the \"Prime\" project and is distributed under \"Prime GPLv1\" license.");
	logger_info(" ");
}
