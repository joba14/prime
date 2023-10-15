
/**
 * @file prime_cli_parser.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#ifndef __PRIME_COMMON__INCLUDE__PRIME_COMMON__PARSER_CLI_PARSER_H_
#define __PRIME_COMMON__INCLUDE__PRIME_COMMON__PARSER_CLI_PARSER_H_

#include <stdint.h>

void prime_cli_parser_initialize(
	const char* const program,
	const char* const description,
	const char* const epilog);

void prime_cli_parser_add_option(
	const char* const name,
	const char** const bind,
	const char* const help);

void prime_cli_parser_parse(
	const uint32_t argc,
	const char** const argv);

void prime_cli_parser_usage(
	void);

#endif
