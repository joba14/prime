
/**
 * @file main.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-11
 */

#include <main.h>

#include <primec/version.h>
#include <primec/debug.h>
#include <primec/logger.h>
#include <primec/token.h>
#include <primec/lexer.h>

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

static void usage(
	const char* const program);

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const arch,
	const char** const entry,
	const char** const output);

static FILE* is_file_valid(
	const char* const file_path);

int32_t main(
	const int32_t argc,
	const char** const argv)
{
	const char* arch = NULL;
	const char* entry = "main";
	const char* output = NULL;

	{
		const int32_t code = parse_command_line(argc, argv, &arch, &entry, &output);
		if (code <= 0) { return code; }
	}

	// TODO: handle these options:
	(void)arch;
	(void)entry;
	(void)output;

	const char** const source_files = argv + (uint64_t)optind;
	const uint64_t source_files_count = (uint64_t)argc - (uint64_t)optind;

	if (source_files_count <= 0)
	{
		primec_logger_error("no source files were provided - see '--help' for usage.");
		return -1;
	}

	for (uint64_t index = 0; index < source_files_count; ++index)
	{
		const char* const source_file_path = source_files[index];
		primec_debug_assert(source_file_path != NULL);
		FILE* const source_file = is_file_valid(source_file_path);
		if (!source_file) { continue; }

		primec_logger_info("file %s if good to go!", source_file_path);

#if 1
{ // TODO: parse the source file here!
		primec_lexer_s lexer = primec_lexer_from_parts(
			source_file_path, source_file
		);

		(void)lexer;
		/*
		primec_token_s token;
		while (primec_lexer_lex(&lexer, &token) != primec_token_none)
		{
			primec_logger_info("%lu", (uint64_t)token.type);
		}
		*/
}
#endif

		fclose(source_file);
	}

	return 0;
}

static void usage(
	const char* const program)
{
	primec_debug_assert(program != NULL);
	primec_logger_log(
		"usage: %s [options] <files...>\n"
		"\n"
		"options:\n"
		"    -h, --help          print the help message\n"
		"    -v, --version       print version and exit\n"
		"    -a, --arch          set the target architecture out of:\n"
		"                        [unixc, windowsc]\n"
		"    -e, --entry         set the entry symbol\n"
		"    -o, --output        set output file name\n"
		"\n"
		"notice:\n"
		"    this is distributed under the \"prime gplv1\" license.\n"
		,
		program
	);
}

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const arch,
	const char** const entry,
	const char** const output)
{
	primec_debug_assert(argv != NULL);
	primec_debug_assert(arch != NULL);
	primec_debug_assert(entry != NULL);
	primec_debug_assert(output != NULL);

	static const struct option options[] =
	{
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ "arch", required_argument, 0, 'a' },
		{ "entry", required_argument, 0, 'e' },
		{ "output", required_argument, 0, 'o' },
		{ 0, 0, 0, 0 }
	};

	int32_t opt = -1;
	while ((opt = (int32_t)getopt_long(argc, (char* const *)argv, "hve:a:o:", options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'h':
			{
				usage(argv[0]);
				return 0;
			} break;

			case 'v':
			{
				primec_logger_log("%s " primec_version_fmt, argv[0], primec_version_arg);
				return 0;
			} break;

			case 'a':
			{
				*arch = optarg;
			} break;

			case 'e':
			{
				*entry = optarg;
			} break;

			case 'o':
			{
				*output = optarg;
			} break;

			default:
			{
				primec_logger_error("invalid command line option - see '--help' for usage.");
				return -1;
			} break;
		}
	}

	return 1;
}

static FILE* is_file_valid(
	const char* const file_path)
{
	FILE* const source_file = fopen(file_path, "rt");
	struct stat source_file_stats;

	if (source_file
		&& (fstat(fileno(source_file), &source_file_stats) == 0)
		&& (S_ISDIR(source_file_stats.st_mode) != 0))
	{
		primec_logger_error("unable to open %s for reading - it is a directory", file_path);
		return NULL;
	}

	if (!source_file)
	{
		primec_logger_error("unable to open %s for reading - no such file", file_path);
		return NULL;
	}

	return source_file;
}
