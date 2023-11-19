
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

#include <primec/utf8.h>
#include <primec/token.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	FILE* file;
	primec_location_s location;
	primec_token_s token;

	struct
	{
		char* data;
		uint64_t capacity;
		uint64_t length;
	} buffer;

	utf8char_t c[2];
	bool require_int;
} primec_lexer_s;

primec_lexer_s primec_lexer_from_parts(
	const char* const file_path,
	FILE* const file);

void primec_lexer_destroy(
	primec_lexer_s* const lexer);

/**
 * @brief Lex next token from the file source.
 * 
 * @warning Once the lexer reaches the end of file token it will keep returning it!
 * It is left for the user of this function to handle this case.
 * 
 * @note The easiest way to do this is to check if a returned token has  either the
 * "primec_token_type_none" type or "primec_token_type_eof" type and stop lexing if
 * that happens. I have added a helper function just for this - to verify the token
 * and determine if one needs to stop lexing or not:
 * see @ref primec_lexer_should_stop_lexing()
 */
primec_token_type_e primec_lexer_lex(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

bool primec_lexer_should_stop_lexing(
	const primec_token_type_e type);

void primec_lexer_unlex(
	primec_lexer_s* const lexer,
	const primec_token_s* const token);

#endif
