
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

static const char* const g_usage_banner =
	"usage: %s [options] <files...>\n"
	"\n"
	"options:\n"
	"    -h, --help          print the help message\n"
	"    -v, --version       print version and exit\n"
	"    -e, --entry         set the entry symbol\n"
	"    -o, --output        set output file name\n"
	"\n"
	"notice:\n"
	"    this executable is distributed under the \"prime gplv1\" license.\n";

static void usage(
	const char* const program);

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const entry,
	const char** const output);

static FILE* is_file_valid(
	const char* const file_path);

int32_t main(
	const int32_t argc,
	const char** const argv)
{
	const char* entry = "main";
	const char* output = NULL;

	const int32_t options_index = parse_command_line(argc, argv, &entry, &output);
	if (options_index <= 0) { return options_index; }

	const char** const source_files = argv + (uint64_t)options_index;
	const uint64_t source_files_count = (uint64_t)argc - (uint64_t)options_index;

	if (source_files_count <= 0)
	{
		primec_logger_error("no source files were provided -- see '--help'.");
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

		primec_token_s token;
		while (primec_lexer_lex(&lexer, &token) != primec_token_type_none)
		{
			// primec_logger_info("%s", primec_token_type_to_string(token.type));
			primec_logger_info("%s", primec_token_to_string(&token));
			(void)getchar();

			if (token.type == primec_token_type_eof)
			{
				break;
			}
		}
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
	primec_logger_log(g_usage_banner, program);
}

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const entry,
	const char** const output)
{
	primec_debug_assert(argv != NULL);
	primec_debug_assert(entry != NULL);
	primec_debug_assert(output != NULL);

	typedef struct option option_s;
	static const option_s options[] =
	{
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ "entry", required_argument, 0, 'e' },
		{ "output", required_argument, 0, 'o' },
		{ 0, 0, 0, 0 }
	};

	int32_t opt = -1;
	while ((opt = (int32_t)getopt_long(argc, (char* const *)argv, "hve:o:", options, NULL)) != -1)
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

			case 'e':
			{
				*entry = (const char*)optarg;
			} break;

			case 'o':
			{
				*output = (const char*)optarg;
			} break;

			default:
			{
				primec_logger_error("invalid command line option -- see '--help'.");
				return -1;
			} break;
		}
	}

	return (int32_t)optind;
}

static FILE* is_file_valid(
	const char* const file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path, &file_stats) != 0)
	{
		switch (errno)
		{
			case ENOENT:
			{
				primec_logger_error("unable to open %s for reading -- file not found", file_path);
			} break;

			case EACCES:
			{
				primec_logger_error("unable to open %s for reading -- permission denied", file_path);
			} break;

			case ENAMETOOLONG:
			{
				primec_logger_error("unable to open %s for reading -- path name exceeds the system-defined maximum length", file_path);
			} break;

			default:
			{
				primec_logger_error("unable to open %s for reading -- failed to stat", file_path);
			} break;
		}

		return NULL;
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		primec_logger_error("unable to open %s for reading -- it is a directory", file_path);
		return NULL;
	}

// TODO: review this block (7):
#if support_reg_stat_mode
	if (!S_ISREG(file_stats.st_mode))
	{
		primec_logger_error("unable to open %s for reading -- it is an irregular file", file_path);
		return NULL;
	}
#endif

	FILE* const file = fopen(file_path, "rt");

	if (NULL == file)
	{
		primec_logger_error("unable to open %s for reading -- failed to open", file_path);
		return NULL;
	}

	return file;
}
