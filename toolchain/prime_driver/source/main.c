
/**
 * @file main.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#include <prime_common/prime_asserts.h>
#include <prime_common/prime_logger.h>
#include <prime_common/prime_cli_parser.h>

#include <stdio.h>

signed int main(
	const signed int argc,
	const char** const argv);

signed int main(
	const signed int argc,
	const char** const argv)
{
	prime_logger_info("hello, prime_driver!\n");

	prime_cli_parser_initialize(argv[0],
		"description:\n  this executable creates intermediate representation\n  (IR) files from prime source code files.",
		"copyright notice:\n  this executable is part of the \"Prime\" project and\n  is distributed under \"Prime GPLv1\" license."
	);

	const char* test_flag = NULL;
	prime_cli_parser_add_option("--test-arg", &test_flag, "Test flag is an argument that helps to figure out if this thing works.");
	prime_cli_parser_parse((uint32_t)argc, argv);

	fprintf(stdout, "test_flag=%s\n", test_flag);

	return 0;
}
