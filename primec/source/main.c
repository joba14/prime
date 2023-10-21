
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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

define_heap_buffer_type(arguments_buffer, string_view_t)

#define CSTR2SV(_cstring) { .data = _cstring, .length = ((uint64_t)(sizeof(_cstring) / sizeof(char)) - 1) }
static cli_option_descriptor_t g_cli_option_descriptors[cli_option_types_count] =
{
	[cli_option_type_help] =
	{
		.short_name  = CSTR2SV("-h"),
		.long_name   = CSTR2SV("--help"),
		.description = CSTR2SV("print this usage/help message.")
	},
	[cli_option_type_with_outputs] =
	{
		.short_name  = CSTR2SV("-wo"),
		.long_name   = CSTR2SV("--with-outputs"),
		.description = CSTR2SV("treat command line arguments in pairs (input and output path pairs).")
	}
};

static void usage(
	const string_view_t program);

signed int main(
	const signed int argc,
	const char** const argv)
{
	const string_view_t program = string_view_from_cstring(argv[0]);
	const uint64_t argc_count_without_program_name = (uint64_t)(argc - 1);

	if (argc_count_without_program_name <= 0)
	{
		logger_error("no command line arguments were provided!");
		usage(program);
		exit(-1);
	}

	arguments_buffer_t arguments =
		arguments_buffer_from_parts(argc_count_without_program_name);

	for (uint64_t index = 0; index < argc_count_without_program_name; ++index)
	{
		arguments_buffer_append(&arguments,
			string_view_from_cstring(argv[index + 1])
		);
	}

	bool with_outputs = false;
	string_view_t* first_argument = arguments_buffer_at(&arguments, 0);
	debug_assert(first_argument != NULL);

	if (string_view_equal(*first_argument, g_cli_option_descriptors[cli_option_type_help].short_name)
	 || string_view_equal(*first_argument, g_cli_option_descriptors[cli_option_type_help].long_name))
	{
		usage(program);
		exit(0);
	}
	else if (string_view_equal(*first_argument, g_cli_option_descriptors[cli_option_type_with_outputs].short_name)
		  || string_view_equal(*first_argument, g_cli_option_descriptors[cli_option_type_with_outputs].long_name))
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
			logger_error("odd amount of command line argumets provided with option `" sv_fmt "`!",
				sv_arg(*first_argument));
			usage(program);
			exit(-1);
		}
	}

	for (uint64_t index = (with_outputs ? 1 : 0); index < arguments.count; ++index)
	{
		const string_view_t* const input_file_path = arguments_buffer_at(&arguments, index);
		debug_assert(input_file_path != NULL);

		const string_view_t* output_file_path = input_file_path;

		if (with_outputs)
		{
			output_file_path = arguments_buffer_at(&arguments, ++index);
		}

		logger_info(sv_fmt " -> " sv_fmt,
			sv_arg(*input_file_path), sv_arg(*output_file_path)
		);
	}

	return 0;
}

static void usage(
	const string_view_t program)
{
	debug_assert_string_view(program);

	logger_info("usage: " sv_fmt " [options] <paths...>", sv_arg(program));
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

		logger_info("    " sv_fmt ", " sv_fmt "\n        " sv_fmt,
			sv_arg(cli_option_descriptor->short_name),
			sv_arg(cli_option_descriptor->long_name),
			sv_arg(cli_option_descriptor->description)
		);
	}
	logger_info(" ");

	logger_info("copyright notice:");
	logger_info("this executable is part of the \"Prime\" project and is distributed under \"Prime GPLv1\" license.");
	logger_info(" ");
}
