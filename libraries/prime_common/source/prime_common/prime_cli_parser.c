
/**
 * @file prime_cli_parser.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#include <prime_common/prime_cli_parser.h>

#include <prime_common/prime_asserts.h>
#include <prime_common/prime_logger.h>

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifndef PRIME_CLI_PARSER_ARGUMENTS_CAPACITY
#	define PRIME_CLI_PARSER_ARGUMENTS_CAPACITY 64
#endif

typedef struct
{
	const char* name;
	const char** bind;
	const char* help;
	bool parsed;
} prime_cli_option_t;

typedef struct
{
	const char* program;
	const char* description;
	const char* epilog;
	prime_cli_option_t arguments[PRIME_CLI_PARSER_ARGUMENTS_CAPACITY];
	uint32_t arguments_count;
} prime_cli_parser_t;

static prime_cli_parser_t g_cli_parser;

void prime_cli_parser_initialize(
	const char* const program,
	const char* const description,
	const char* const epilog)
{
	prime_asserts_assert(program != NULL);

	g_cli_parser.program = program;
	g_cli_parser.description = description;
	g_cli_parser.epilog = epilog;

	(void)memset(g_cli_parser.arguments, 0,
		PRIME_CLI_PARSER_ARGUMENTS_CAPACITY * sizeof(prime_cli_option_t));
	g_cli_parser.arguments_count = 0;
}

void prime_cli_parser_add_option(
	const char* const name,
	const char** const bind,
	const char* const help)
{
	prime_asserts_assert(name != NULL);
	prime_asserts_assert(bind != NULL);
	prime_asserts_assert(help != NULL);

	prime_asserts_assert(g_cli_parser.arguments_count + 1 < PRIME_CLI_PARSER_ARGUMENTS_CAPACITY);
	g_cli_parser.arguments[g_cli_parser.arguments_count++] = (prime_cli_option_t)
	{
		.name = name,
		.bind = bind,
		.help = help,
		.parsed = false
	};
}

void prime_cli_parser_parse(
	const uint32_t argc,
	const char** const argv)
{
	prime_asserts_assert(argv != NULL);
	prime_asserts_assert(argc > 0);

	// First, check if there are sufficient amount of args:
	if ((argc - 1) < g_cli_parser.arguments_count)
	{
		prime_logger_error("insufficient amount of command line arguments were provided!");
		prime_cli_parser_usage();
		exit(-1);
	}

	for (const char** argv_iterator = (argv + 1); *argv_iterator != NULL; ++argv_iterator)
	{
		const char* argv_argument = *argv_iterator;
		prime_asserts_assert(argv_argument != NULL);
		const uint32_t argv_argument_length = (uint32_t)strlen(argv_argument);
		prime_cli_option_t* cli_argument = NULL;

		for (uint32_t index = 0; index < g_cli_parser.arguments_count; ++index)
		{
			const prime_cli_option_t* const temp_cli_argument = &g_cli_parser.arguments[index];
			prime_asserts_assert(temp_cli_argument != NULL);
			const uint32_t temp_cli_argument_length = (uint32_t)strlen(temp_cli_argument->name);

			if (temp_cli_argument_length == argv_argument_length
			 && strncmp(temp_cli_argument->name, argv_argument, temp_cli_argument_length) == 0)
			{
				cli_argument = (prime_cli_option_t*)temp_cli_argument;
				break;
			}
		}

		if (cli_argument == NULL)
		{
			continue;
		}

		if (cli_argument->parsed)
		{
			prime_logger_error("encountered a duplicate command line arguments `%s`!", cli_argument->name);
			prime_cli_parser_usage();
			exit(-1);
		}

		if (*(argv_iterator + 1) == NULL)
		{
			prime_logger_error("missing a value for command line argument `%s`!", cli_argument->name);
			prime_cli_parser_usage();
			exit(-1);
		}

		const char* next_argv_argument = *(++argv_iterator);
		prime_asserts_assert(next_argv_argument != NULL);
		*cli_argument->bind = next_argv_argument;
		cli_argument->parsed = true;
	}

	for (uint32_t index = 0; index < g_cli_parser.arguments_count; ++index)
	{
		prime_cli_option_t* const cli_argument = &g_cli_parser.arguments[index];
		prime_asserts_assert(cli_argument != NULL);

		if (!cli_argument->parsed)
		{
			prime_logger_error("missing command line arguments `%s`!", cli_argument->name);
			prime_cli_parser_usage();
			exit(-1);
		}
	}
}

void prime_cli_parser_usage(
	void)
{
	prime_logger_info("usage: %s [options]\n", g_cli_parser.program);

	if (g_cli_parser.description != NULL)
	{
		prime_logger_info("%s\n", g_cli_parser.description);
	}

	prime_logger_info("options:");
	for (uint32_t index = 0; index < g_cli_parser.arguments_count; ++index)
	{
		const prime_cli_option_t* const argument =
			(const prime_cli_option_t* const)&g_cli_parser.arguments[index];
		prime_asserts_assert(argument != NULL);

		prime_logger_info("    %s <value>", argument->name);
		prime_logger_info("        %s", argument->help);
	}
	// prime_logger_info("");

	if (g_cli_parser.epilog != NULL)
	{
		prime_logger_info("\n%s\n", g_cli_parser.epilog);
	}
}
