
/**
 * @file lexer.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-12
 */

#ifndef __primec__include__primec__lexer_h__
#define __primec__include__primec__lexer_h__

#include <primec/token.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	FILE* file;
	primec_location_s location;
	primec_token_s token;
	char* buffer;
	uint64_t buffer_capacity;
	uint64_t buffer_length;
	uint32_t c[2];
	bool require_int;
} primec_lexer_s;

primec_lexer_s primec_lexer_from_parts(
	const char* const file_path,
	FILE* const file);

void primec_lexer_destroy(
	primec_lexer_s* const lexer);

primec_token_type_e primec_lexer_lex(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

void primec_lexer_unlex(
	primec_lexer_s* const lexer,
	const primec_token_s* const token);

#endif
