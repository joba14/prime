
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
#include <primec/logger.h>
#include <primec/utils.h>

#include <inttypes.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static void update_location(
	primec_location_s* const location,
	const utf8char_t utf8char)
{
	primec_debug_assert(location != NULL);

	if ('\n' == utf8char)
	{
		location->line++;
		location->column = 0;
	}
	else
	{
		location->column++;
	}
}

static void append_buffer(
	primec_lexer_s* const lexer,
	const char* const buffer,
	const uint64_t size)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(buffer != NULL);
	primec_debug_assert(size > 0);

	if (lexer->buffer_length + size >= lexer->buffer_capacity)
	{
		lexer->buffer_capacity *= 2;
		lexer->buffer = primec_utils_realloc(lexer->buffer, lexer->buffer_capacity);
	}

	(void)memcpy(lexer->buffer + lexer->buffer_length, buffer, size);
	lexer->buffer_length += size;
	lexer->buffer[lexer->buffer_length] = 0;
}

#define log_lexer_error(_location, _format, ...)                               \
	do {                                                                       \
		(void)fprintf(stderr, "%s:%lu:%lu: ",                                  \
			(_location).file, (_location).line, (_location).column);           \
		primec_logger_error(_format, ## __VA_ARGS__);                          \
		exit(-1);                                                              \
	} while (0)

static utf8char_t next_utf8char(
	primec_lexer_s* const lexer,
	primec_location_s* const location,
	const bool buffer)
{
	primec_debug_assert(lexer != NULL);
	utf8char_t utf8char = primec_utf8_invalid;

	if (lexer->c[0] != primec_utf8_invalid)
	{
		utf8char = lexer->c[0];
		lexer->c[0] = lexer->c[1];
		lexer->c[1] = primec_utf8_invalid;
	}
	else
	{
		utf8char = primec_utf8_get(lexer->file);
		update_location(&lexer->location, utf8char);

		if (primec_utf8_invalid == utf8char && !feof(lexer->file))
		{
			log_lexer_error(lexer->location, "invalid utf-8 sequence encountered");
		}
	}

	if (location != NULL)
	{
		*location = lexer->location;

		for (uint8_t index = 0; index < 2 && lexer->c[index] != primec_utf8_invalid; ++index)
		{
			update_location(&lexer->location, lexer->c[index]);
		}
	}

	if (primec_utf8_invalid == utf8char || !buffer)
	{
		return utf8char;
	}

	char utf8_buffer[primec_utf8_max_size];
	const uint8_t size = primec_utf8_encode(utf8_buffer, utf8char);
	append_buffer(lexer, utf8_buffer, size);
	return utf8char;
}

static bool is_white_space(
	const utf8char_t utf8char)
{
	return '\t' == utf8char || '\n' == utf8char || '\r' == utf8char || ' ' == utf8char;
}

static utf8char_t get_utf8char(
	primec_lexer_s* const lexer,
	primec_location_s* const location)
{
	primec_debug_assert(lexer != NULL);
	utf8char_t utf8char = primec_utf8_invalid;
	while ((utf8char = next_utf8char(lexer, location, false)) != primec_utf8_invalid && is_white_space(utf8char));
	return utf8char;
}

static void clear_buffer(
	primec_lexer_s* const lexer)
{
	primec_debug_assert(lexer != NULL);
	lexer->buffer_length = 0;
	lexer->buffer[0] = 0;
}

static void consume_buffer(
	primec_lexer_s* const lexer,
	const uint64_t length)
{
	for (uint64_t index = 0; index < length; ++index)
	{
		while (0x80 == (lexer->buffer[--lexer->buffer_length] & 0xC0));
	}

	lexer->buffer[lexer->buffer_length] = 0;
}

static void push_utf8char(
	primec_lexer_s* const lexer,
	const utf8char_t utf8char,
	const bool buffer)
{
	primec_debug_assert(lexer != NULL);

	primec_debug_assert(primec_utf8_invalid == lexer->c[1]);
	lexer->c[1] = lexer->c[0];
	lexer->c[0] = utf8char;

	if (buffer)
	{
		consume_buffer(lexer, 1);
	}
}

static primec_token_type_e lex_identifier_or_keyword(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	utf8char_t utf8char = next_utf8char(lexer, &token->location, true);
	primec_debug_assert(token != NULL);

	// TODO: make it not an assert?
	primec_debug_assert(utf8char != primec_utf8_invalid
					 && utf8char <= 0x7F
					 && (isalpha(utf8char) || utf8char == '_'));

	while ((utf8char = next_utf8char(lexer, NULL, true)) != primec_utf8_invalid)
	{
		if (utf8char > 0x7F || (!isalnum(utf8char) && utf8char != '_'))
		{
			push_utf8char(lexer, utf8char, true);
			break;
		}
	}

	token->type = primec_token_type_from_string(lexer->buffer);
	token->source.data = primec_utils_strdup(lexer->buffer);
	token->source.length = lexer->buffer_length;

	clear_buffer(lexer);
	return token->type;
}

primec_lexer_s primec_lexer_from_parts(
	const char* const file_path,
	FILE* const file)
{
	primec_debug_assert(file_path != NULL);
	primec_debug_assert(file != NULL);

	primec_lexer_s lexer;
	lexer.file = file;
	lexer.token.type = primec_token_type_none;
	lexer.location.file = file_path;
	lexer.location.line = 1;
	lexer.location.column = 0;

	lexer.buffer_capacity = 256;
	lexer.buffer = primec_utils_malloc(lexer.buffer_capacity * sizeof(char));
	primec_debug_assert(lexer.buffer != NULL);
	lexer.buffer_length = 0;
	lexer.c[0] = primec_utf8_invalid;
	lexer.c[1] = primec_utf8_invalid;
	lexer.require_int = false;
	return lexer;
}

void primec_lexer_destroy(
	primec_lexer_s* const lexer)
{
	primec_utils_free(lexer->buffer);
	memset((void*)lexer, 0, sizeof(primec_lexer_s));
}

primec_token_type_e primec_lexer_lex(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	if (lexer->token.type != primec_token_type_none)
	{
		*token = lexer->token;
		lexer->token.type = primec_token_type_none;
		return token->type;
	}

	utf8char_t c = get_utf8char(lexer, &token->location);

	if (primec_utf8_invalid == c)
	{
		token->type = primec_token_type_eof;
		token->source.length = 0;
		return token->type;
	}

	/*
	if (c <= 0x7F && isdigit(c))
	{
		push(lexer, c, false);
		lex_literal(lexer, token);
		return token->type;
	}

	lexer->require_int = false;
	*/

	if (c <= 0x7F && (isalpha(c) || c == '_'))
	{
		push_utf8char(lexer, c, false);
		return lex_identifier_or_keyword(lexer, token);
	}

	/*
	switch (c)
	{
		case '"':
		case '`':
		case '\'':
		{
			push(lexer, c, false);
			return lex_string(lexer, token);
		} break;

		case '.': // . .. ...
		case '<': // < << <= <<=
		case '>': // > >> >= >>=
		case '&': // & && &= &&=
		case '|': // | || |= ||=
		case '^': // ^ ^^ ^= ^^=
		{
			return lex3(lexer, token, c);
		} break;

		case '*': // * *=
		case '%': // % %=
		case '/': // / /= //
		case '+': // + +=
		case '-': // - -=
		case ':': // : ::
		case '!': // ! !=
		case '=': // = == =>
		{
			return lex2(lexer, token, c);
		} break;

		case '~':
		{
			token->type = primec_token_type_bnot;
		} break;

		case ',':
		{
			token->type = primec_token_type_comma;
		} break;

		case '{':
		{
			token->type = primec_token_type_left_brace;
		} break;

		case '[':
		{
			token->type = primec_token_type_left_bracket;
		} break;

		case '(':
		{
			token->type = primec_token_type_left_parenth;
		} break;

		case '}':
		{
			token->type = primec_token_type_right_brace;
		} break;

		case ']':
		{
			token->type = primec_token_type_right_bracket;
		} break;

		case ')':
		{
			token->type = primec_token_type_right_parenth;
		} break;

		case ';':
		{
			token->type = primec_token_type_semicolon;
		} break;

		default:
		{
			// primec_logger_panic("%s:%lu:%lu: syntax error: unexpected codepoint '%s'\n",
			// 	lexer->location.file, lexer->location.line,
			// 	lexer->location.column, rune_unparse(c));
		} break;
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
	primec_debug_assert(token->type != primec_token_type_none);
	lexer->token = *token;
}
