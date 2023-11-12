
/**
 * @file lexer.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-12
 */

#include <primec/lexer.h>

#include <primec/debug.h>

#include <stddef.h>
#include <memory.h>
#include <stdlib.h>

primec_lexer_s primec_lexer_from_parts(
	const char* const file_path,
	FILE* const file)
{
	primec_debug_assert(file_path != NULL);
	primec_debug_assert(file != NULL);

	primec_lexer_s lexer;
	memset(&lexer, 0, sizeof(primec_lexer_s));

	lexer.file = file;
	lexer.buffer_capacity = 256;

	lexer.buffer = (char*)malloc(lexer.buffer_capacity * sizeof(char));
	primec_debug_assert(lexer.buffer != NULL);

	lexer.location.file = file_path;
	lexer.location.line = 1;
	lexer.location.column = 0;

	lexer.c[0] = UINT32_MAX;
	lexer.c[1] = UINT32_MAX;
	// lexer.un.token = T_NONE;

	return lexer;
}

void primec_lexer_cleanup(
	primec_lexer_s* const lexer)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(lexer->file != NULL);
	primec_debug_assert(lexer->buffer != NULL);
	free(lexer->buffer);
}

primec_token_e primec_lexer_lex(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	if (lexer->token.type != primec_token_none)
	{
		*token = lexer->token;
		lexer->token.type = primec_token_none;
		return token->type;
	}

	const int32_t c = getc(lexer->file);

	switch (c)
	{
		case '#':
		{
			// ... 
		} break;

		default:
		{
		} break;
	}

	/*
	uint32_t c = wgetc(lexer, &token->location);
	if (c == primec_token_eof)
	{
		token->type = primec_token_eof;
		return token->type;
	}

	if (c <= 0x7F && isdigit(c))
	{
		push(lexer, c, false);
		lex_literal(lexer, token);
		return T_LITERAL;
	}

	lexer->require_int = false;

	if (c <= 0x7F && (isalpha(c) || c == '_' || c == '@'))
	{
		push(lexer, c, false);
		return lex_name(lexer, token);
	}

	switch (c)
	{
		case '"':
		case '`':
		case '\'':
			push(lexer, c, false);
			return lex_string(lexer, token);
		case '.': // . .. ...
		case '<': // < << <= <<=
		case '>': // > >> >= >>=
		case '&': // & && &= &&=
		case '|': // | || |= ||=
		case '^': // ^ ^^ ^= ^^=
			return lex3(lexer, out, c);
		case '*': // * *=
		case '%': // % %=
		case '/': // / /= //
		case '+': // + +=
		case '-': // - -=
		case ':': // : ::
		case '!': // ! !=
		case '=': // = == =>
			return lex2(lexer, out, c);
		case '~':
			out->token = T_BNOT;
			break;
		case ',':
			out->token = T_COMMA;
			break;
		case '{':
			out->token = T_LBRACE;
			break;
		case '[':
			out->token = T_LBRACKET;
			break;
		case '(':
			out->token = T_LPAREN;
			break;
		case '}':
			out->token = T_RBRACE;
			break;
		case ']':
			out->token = T_RBRACKET;
			break;
		case ')':
			out->token = T_RPAREN;
			break;
		case ';':
			out->token = T_SEMICOLON;
			break;
		case '?':
			out->token = T_QUESTION;
			break;
		default:
			xfprintf(stderr, "%s:%d:%d: syntax error: unexpected codepoint '%s'\n",
				sources[lexer->location.file], lexer->location.line,
				lexer->location.column, rune_unparse(c));
			exit(EXIT_FAILURE);
	}
	*/

	return token->type;
}

void primec_lexer_unlex(
	primec_lexer_s* const lexer,
	const primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);
	primec_debug_assert(token->type != primec_token_none);
	lexer->token = *token;
}
