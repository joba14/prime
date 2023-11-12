
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
	char buffer[256];
	uint64_t buffer_length;
	primec_token_s token;
	primec_location_s location;
} primec_lexer_s;

primec_lexer_s primec_lexer_from_parts(
	const char* const file_path,
	FILE* const file);

primec_token_e primec_lexer_lex(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

void primec_lexer_unlex(
	primec_lexer_s* const lexer,
	const primec_token_s* const token);

#endif
