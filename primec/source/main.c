
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
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

static void usage(const char* const program);
static void usage(const char* const program)
{
	primec_debug_assert(program != NULL);
	primec_logger_log(
		"usage: %s [options] <files...>\n"
		"\n"
		"options:\n"
		"    -h, --help          print the help message\n"
		"    -v, --version       print version and exit\n"
		"    -e, --entry         set the entry symbol\n"
		"    -o, --output        set output file name\n"
		"\n"
		"notice:\n"
		"    this is distributed under the \"prime gplv1\" license.\n"
		,
		program
	);
}

signed int main(
	const signed int argc,
	const char** const argv)
{
	const char* entry = "main";
	const char* output = NULL;

	static const struct option options[] =
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
				entry = optarg;
				break;
			} break;

			case 'o':
			{
				output = optarg;
			} break;

			default:
			{
				primec_logger_error("invalid command line option - see '--help' for usage.");
				return -1;
			} break;
		}
	}

	const char** const sources = argv + (uint64_t)optind;
	const uint64_t sources_count = (uint64_t)argc - (uint64_t)optind;

	if (sources_count <= 0)
	{
		primec_logger_error("no source files were provided!");
		usage(argv[0]);
		return -1;
	}

	for (const char** sources_iterator = sources; *sources_iterator; ++sources_iterator)
	{
		const char* const source_path = *sources_iterator;
		FILE* const source_file = fopen(source_path, "rt");
		struct stat source_file_stats;

		if (source_file
		 && (fstat(fileno(source_file), &source_file_stats) == 0)
		 && (S_ISDIR(source_file_stats.st_mode) != 0))
		{
			primec_logger_error("unable to open %s for reading - it is a directory", source_path);
			continue;
		}

		if (!source_file)
		{
			primec_logger_error("unable to open %s for reading: %s", source_path, strerror(errno));
			continue;
		}

		primec_logger_info("file %s if good to go!", source_path);

{ // TODO: parse the source file here!
		primec_lexer_s lexer = primec_lexer_from_parts(
			source_path, source_file
		);

		primec_token_s token;
		while (primec_lexer_lex(&lexer, &token) != primec_token_none)
		{
			primec_logger_info("%lu", (uint64_t)token.type);
		}

		primec_lexer_cleanup(&lexer);
}

		fclose(source_file);

		/*
		lex_init(&lexer, in,  i + 1);
		parse(&lexer, subunit);
		if (i + 1 < nsources) {
			*next = xcalloc(1, sizeof(struct ast_subunit));
			subunit = *next;
			next = &subunit->next;
		}
		lex_finish(&lexer);
		*/
	}

	printf("entry: %s\n", entry);
	printf("output: %s\n", output);

	return 0;
}
