
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
#include <errno.h>
#include <stdio.h>

#define log_lexer_error(_location, _format, ...)                               \
	do {                                                                       \
		(void)fprintf(stderr, "%s:%lu:%lu: ",                                  \
			(_location).file, (_location).line, (_location).column);           \
		primec_logger_error(_format, ## __VA_ARGS__);                          \
		exit(-1);                                                              \
	} while (0)

static void update_location(
	primec_location_s* const location,
	const utf8char_t utf8char);

static void append_buffer(
	primec_lexer_s* const lexer,
	const char* const buffer,
	const uint64_t size);

static utf8char_t next_utf8char(
	primec_lexer_s* const lexer,
	primec_location_s* const location,
	const bool buffer);

static bool is_symbol_a_white_space(
	const utf8char_t utf8char);

static utf8char_t get_utf8char(
	primec_lexer_s* const lexer,
	primec_location_s* const location);

static void clear_buffer(
	primec_lexer_s* const lexer);

static void consume_buffer(
	primec_lexer_s* const lexer,
	const uint64_t length);

static void push_utf8char(
	primec_lexer_s* const lexer,
	const utf8char_t utf8char,
	const bool buffer);

static bool is_symbol_first_of_identifier_or_keyword(
	const utf8char_t utf8char);

static bool is_symbol_not_first_of_identifier_or_keyword(
	const utf8char_t utf8char);

static bool is_symbol_first_of_numeric_literal(
	const utf8char_t utf8char);

static primec_token_type_e lex_identifier_or_keyword(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

static bool lex_numeric_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const out);

static uint8_t lex_possible_rune(
	primec_lexer_s* const lexer,
	char* rune);

static primec_token_type_e lex_rune_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

static primec_token_type_e lex_string_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token);

static primec_token_type_e lex_up_to_2_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	utf8char_t utf8char);

static primec_token_type_e lex_up_to_3_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	uint32_t utf8char);

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
	primec_utils_memset((void*)lexer, 0, sizeof(primec_lexer_s));
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

	utf8char_t utf8char = get_utf8char(lexer, &token->location);

	if (primec_utf8_invalid == utf8char)
	{
		lexer->token = primec_token_from_parts(primec_token_type_eof, lexer->location);
		*token = lexer->token;
		return token->type;
	}

	if (utf8char <= 0x7F && (isdigit(utf8char) || '+' == utf8char || '-' == utf8char))
	{
		push_utf8char(lexer, utf8char, false);

		if (lex_numeric_literal_token(lexer, token))
		{
			return token->type;
		}
	}

	lexer->require_int = false;

	if (is_symbol_first_of_identifier_or_keyword(utf8char))
	{
		push_utf8char(lexer, utf8char, false);
		return lex_identifier_or_keyword(lexer, token);
	}

	switch (utf8char)
	{
		case '\'':
		{
			push_utf8char(lexer, utf8char, false);
			return lex_rune_literal_token(lexer, token);
		} break;

		case '\"':
		{
			push_utf8char(lexer, utf8char, false);
			return lex_string_literal_token(lexer, token);
		} break;

		case '<':
		case '>':
		case '&':
		case '|':
		case '^':
		{
			return lex_up_to_3_symbol_token(lexer, token, utf8char);
		} break;

		case '.':
		case '*':
		case '%':
		case '/':
		case '+':
		case '-':
		case ':':
		case '!':
		case '=':
		{
			return lex_up_to_2_symbol_token(lexer, token, utf8char);
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
			char invalid[4];
			const uint8_t length = primec_utf8_encode(invalid, utf8char);
			log_lexer_error(token->location, "invalid token encountered: `%.*s`",
				(signed int)length, invalid
			);
		} break;
	}

	return token->type;
}

bool primec_lexer_should_stop_lexing(
	const primec_token_type_e type)
{
	return primec_token_type_none == type || primec_token_type_eof == type;
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
			log_lexer_error(lexer->location, "invalid utf-8 sequence encountered.");
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

static bool is_symbol_a_white_space(
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
	while ((utf8char = next_utf8char(lexer, location, false)) != primec_utf8_invalid && is_symbol_a_white_space(utf8char));
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

static bool is_symbol_first_of_identifier_or_keyword(
	const utf8char_t utf8char)
{
	primec_debug_assert(utf8char != primec_utf8_invalid);
	return (utf8char <= 0x7F) && (isalpha(utf8char) || '_' == utf8char);
}

static bool is_symbol_not_first_of_identifier_or_keyword(
	const utf8char_t utf8char)
{
	primec_debug_assert(utf8char != primec_utf8_invalid);
	return (utf8char <= 0x7F) && (isalnum(utf8char) || '_' == utf8char);
}

static bool is_symbol_first_of_numeric_literal(
	const utf8char_t utf8char)
{
	primec_debug_assert(utf8char != primec_utf8_invalid);
	return (utf8char <= 0x7F) && (isdigit(utf8char) || '+' == utf8char || '-' == utf8char);
}

static primec_token_type_e lex_identifier_or_keyword(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	utf8char_t utf8char = next_utf8char(lexer, &token->location, true);
	primec_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(
		is_symbol_first_of_identifier_or_keyword(utf8char)
	);// Sanity check for developers.

	while ((utf8char = next_utf8char(lexer, NULL, true)) != primec_utf8_invalid)
	{
		if (!is_symbol_not_first_of_identifier_or_keyword(utf8char))
		{
			push_utf8char(lexer, utf8char, true);
			break;
		}
	}

	token->type = primec_token_type_from_string(lexer->buffer);
	token->ident.data = primec_utils_strndup(lexer->buffer, lexer->buffer_length);
	token->ident.length = lexer->buffer_length;

	clear_buffer(lexer);
	return token->type;
}

static bool lex_numeric_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const out)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(out != NULL);

	enum
	{
		base_bin = 1, base_oct, base_hex, base_dec = 0x07, base_mask = base_dec
	};

	_Static_assert(
		(base_bin | base_oct | base_hex | base_dec) == base_dec, "base_dec bits must be a superset of all other bases"
	);

	enum
	{
		flag_flt = 3, flag_suff, flag_dig
	};

	static const char chrs[][24] =
	{
		[base_bin] = "01",
		[base_oct] = "01234567",
		[base_dec] = "0123456789",
		[base_hex] = "0123456789abcdefABCDEF"
	};

	static const char matching_states[0x80][6] =
	{
		['.'] = { base_dec, base_hex, 0 },
		['e'] = { base_dec, base_dec | 1 << flag_flt, 0 },
		['E'] = { base_dec, base_dec | 1 << flag_flt, 0 },
		['p'] = { base_hex, base_hex | 1 << flag_flt, 0 },
		['P'] = { base_hex, base_hex | 1 << flag_flt, 0 },
		['+'] = { base_dec | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_dig, 0 },
		['-'] = { base_dec | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_dig, 0 },
		['i'] = { base_bin, base_oct, base_hex, base_dec, base_dec, 0 },
		['u'] = { base_bin, base_oct, base_hex, base_dec, base_dec, 0 },
		['f'] = { base_dec, base_dec | 1 << flag_flt, base_dec, base_dec | 1 << flag_flt, 0 },
	};

	int32_t state = base_dec;
	int32_t base = 10;
	int32_t old_state = base_dec;

	utf8char_t utf8char = next_utf8char(lexer, &out->location, true);
	utf8char_t last = 0;

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(
		is_symbol_first_of_numeric_literal(utf8char)
	);// Sanity check for developers.

	utf8char_t sign_symbol = '\0';

	if ('+' == utf8char || '-' == utf8char)
	{
		sign_symbol = utf8char;
		utf8char = next_utf8char(lexer, NULL, true);
	}

	if ('0' == utf8char)
	{
		utf8char = next_utf8char(lexer, NULL, true);

		if (utf8char <= 0x7F && isdigit(utf8char))
		{
			log_lexer_error(out->location, "leading zero in base 10 literal.");
		}

		if ('b' == utf8char)
		{
			if (sign_symbol != '\0')
			{
				log_lexer_error(out->location, "sign cannot prefix binary literal.");
			}

			state = base_bin | 1 << flag_dig;
			base = 2;
		}
		else if ('o' == utf8char)
		{
			if (sign_symbol != '\0')
			{
				log_lexer_error(out->location, "sign cannot prefix octal literal.");
			}

			state = base_oct | 1 << flag_dig;
			base = 8;
		}
		else if ('x' == utf8char)
		{
			if (sign_symbol != '\0')
			{
				log_lexer_error(out->location, "sign cannot prefix hex literal.");
			}

			state = base_hex | 1 << flag_dig;
			base = 16;
		}
	}

	if (state != base_dec)
	{
		last = utf8char;
		utf8char = next_utf8char(lexer, NULL, true);
	}

	uint64_t suffix_start = 0;

	do
	{
		if (strchr(chrs[state & base_mask], (int32_t)utf8char))
		{
			state &= ~(1 << flag_dig);
			last = utf8char;
			continue;
		}
		else if (utf8char > 0x7f || !strchr(matching_states[utf8char], state))
		{
			goto end;
		}

		old_state = state;

		switch (utf8char)
		{
			case '.':
			{
				if (lexer->require_int)
				{
					goto want_int;
				}

				state |= 1 << flag_flt;
			} break;

			case '-':
			case 'p':
			case 'P':
			{
				state |= 1 << flag_flt;
			} /* fallthrough */

			case 'f':
			{
				state |= 1 << flag_flt;
			} /* fallthrough */

			case 'i':
			case 'u':
			{
				state |= base_dec | 1 << suffix_start;
				suffix_start = lexer->buffer_length - 1;
			} break;

			default:
			{
				goto end;
			} break;
		}

		if (state & 1 << flag_flt && lexer->require_int)
		{
			log_lexer_error(out->location, "expected integer literal.");
		}

		last = utf8char;
		state |= 1 << flag_dig;
	} while ((utf8char = next_utf8char(lexer, NULL, true)) != primec_utf8_invalid);

	last = 0;

end:
	if (last && !strchr("iu", (int32_t)last) && !strchr(chrs[state & base_mask], (int32_t)last))
	{
		state = old_state;
		push_utf8char(lexer, utf8char, true);
		push_utf8char(lexer, last, true);
	}
	else if (utf8char != primec_utf8_invalid)
	{
want_int:
		push_utf8char(lexer, utf8char, true);
	}

	lexer->require_int = false;

	typedef enum
	{
		kind_unknown = -1, kind_i8, kind_i16, kind_i32, kind_i64, kind_u8, kind_u16, kind_u32, kind_u64, kind_f32, kind_f64
	} kind_e;

	kind_e kind = kind_unknown;

	static const struct
	{
		const char suffix[4];
		kind_e kind;
		primec_token_type_e type;
	} literals[] =
	{
		{ "i8",  kind_i8,  primec_token_type_literal_i8  },
		{ "i16", kind_i16, primec_token_type_literal_i16 },
		{ "i32", kind_i32, primec_token_type_literal_i32 },
		{ "i64", kind_i64, primec_token_type_literal_i64 },
		{ "u8",  kind_u8,  primec_token_type_literal_u8  },
		{ "u16", kind_u16, primec_token_type_literal_u16 },
		{ "u32", kind_u32, primec_token_type_literal_u32 },
		{ "u64", kind_u64, primec_token_type_literal_u64 },
		{ "f32", kind_f32, primec_token_type_literal_f32 },
		{ "f64", kind_f64, primec_token_type_literal_f64 }
	};

	if (suffix_start)
	{
		for (uint8_t index = 0; index < (sizeof(literals) / sizeof(literals[0])); ++index)
		{
			if (!primec_utils_strcmp(literals[index].suffix, lexer->buffer + suffix_start))
			{
				out->type = literals[index].type;
				kind = literals[index].kind;
				break;
			}
		}

		if (kind_unknown == kind)
		{
			log_lexer_error(out->location, "invalid suffix '%s'.", lexer->buffer + suffix_start);
		}
	}
	else
	{
		return false;
	}

	const int64_t offset = (10 == base ? 0 : 2);
	errno = 0;

	switch (kind)
	{
		case kind_i8:
		{
			const int64_t value = strtoimax(lexer->buffer + offset, NULL, base);
			out->i8 = (int8_t)value;
		} break;

		case kind_i16:
		{
			const int64_t value = strtoimax(lexer->buffer + offset, NULL, base);
			out->i16 = (int16_t)value;
		} break;

		case kind_i32:
		{
			const int64_t value = strtoimax(lexer->buffer + offset, NULL, base);
			out->i32 = (int32_t)value;
		} break;

		case kind_i64:
		{
			const int64_t value = strtoimax(lexer->buffer + offset, NULL, base);
			out->i64 = (int64_t)value;
		} break;

		case kind_u8:
		{
			const uint64_t value = (uint64_t)strtoimax(lexer->buffer + offset, NULL, base);
			out->u8 = (uint8_t)value;
		} break;

		case kind_u16:
		{
			const uint64_t value = (uint64_t)strtoimax(lexer->buffer + offset, NULL, base);
			out->u16 = (uint16_t)value;
		} break;

		case kind_u32:
		{
			const uint64_t value = (uint64_t)strtoimax(lexer->buffer + offset, NULL, base);
			out->u32 = (uint32_t)value;
		} break;

		case kind_u64:
		{
			const uint64_t value = (uint64_t)strtoimax(lexer->buffer + offset, NULL, base);
			out->u64 = (uint64_t)value;
		} break;

		case kind_f32:
		{
			out->f32 = strtof(lexer->buffer + offset, NULL);
		} break;

		case kind_f64:
		{
			out->f64 = strtold(lexer->buffer + offset, NULL);
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function handles unknown
			//       numeric literal kind before entering the switch!
			primec_debug_assert(0); // Sanity check for developers.
		} break;
	}

	clear_buffer(lexer);
	return true;
}

static uint8_t lex_possible_rune(
	primec_lexer_s* const lexer,
	char* rune)
{
	utf8char_t utf8char = next_utf8char(lexer, NULL, false);
	primec_debug_assert(utf8char != primec_utf8_invalid);

	switch (utf8char)
	{
		case '\\':
		{
			primec_location_s loc = lexer->location;
			utf8char = next_utf8char(lexer, NULL, false);

			char buffer[9];
			char* end_pointer;

			switch (utf8char)
			{
				case '0':
				{
					rune[0] = '\0';
					return 1;
				} break;

				case 'a':
				{
					rune[0] = '\a';
					return 1;
				} break;

				case 'b':
				{
					rune[0] = '\b';
					return 1;
				} break;

				case 'f':
				{
					rune[0] = '\f';
					return 1;
				} break;

				case 'n':
				{
					rune[0] = '\n';
					return 1;
				} break;

				case 'r':
				{
					rune[0] = '\r';
					return 1;
				} break;

				case 't':
				{
					rune[0] = '\t';
					return 1;
				} break;

				case 'v':
				{
					rune[0] = '\v';
					return 1;
				} break;

				case '\\':
				{
					rune[0] = '\\';
					return 1;
				} break;

				case '\'':
				{
					rune[0] = '\'';
					return 1;
				} break;

				case '\"':
				{
					rune[0] = '\"';
					return 1;
				} break;

				case 'x':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error(loc, "invalid hex literal.");
					}

					rune[0] = (char)utf8char;
					return 1;
				} break;

				case 'u':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = (char)next_utf8char(lexer, NULL, false);
					buffer[3] = (char)next_utf8char(lexer, NULL, false);
					buffer[4] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error(loc, "invalid hex literal.");
					}

					return primec_utf8_encode(rune, utf8char);
				} break;

				case 'U':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = (char)next_utf8char(lexer, NULL, false);
					buffer[3] = (char)next_utf8char(lexer, NULL, false);
					buffer[4] = (char)next_utf8char(lexer, NULL, false);
					buffer[5] = (char)next_utf8char(lexer, NULL, false);
					buffer[6] = (char)next_utf8char(lexer, NULL, false);
					buffer[7] = (char)next_utf8char(lexer, NULL, false);
					buffer[8] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error(loc, "invalid hex literal.");
					}

					return primec_utf8_encode(rune, utf8char);
				} break;

				case primec_utf8_invalid:
				{
					log_lexer_error(lexer->location, "unexpected end of file.");
				} break;

				default:
				{
					log_lexer_error(loc, "invalid escape '\\%c'.", (char)utf8char);
				} break;
			}

			primec_debug_assert(0);
			return 0;
		} break;

		default:
		{
			return primec_utf8_encode(rune, utf8char);
		} break;
	}

	primec_debug_assert(0);
	return 0;
}

static primec_token_type_e lex_rune_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	utf8char_t utf8char = next_utf8char(lexer, &token->location, false);

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(
		utf8char != primec_utf8_invalid && '\'' == utf8char
	); // Sanity check for developers.

	switch (utf8char)
	{
		case '\'':
		{
			utf8char = next_utf8char(lexer, NULL, false);

			switch (utf8char)
			{
				case '\'':
				{
					log_lexer_error(token->location, "expected rune before trailing single quote.");
				} break;

				case '\\':
				{
					push_utf8char(lexer, utf8char, false);
					const primec_location_s location = lexer->location;

					char buffer[primec_utf8_max_size + 1];
					const uint8_t size = lex_possible_rune(lexer, buffer);
					buffer[size] = '\0';

					const char* rune = buffer;
					token->rune = primec_utf8_decode(&rune);

					if (primec_utf8_invalid == token->rune)
					{
						log_lexer_error(location, "invalid utf-8 in rune literal.");
					}
				} break;

				default:
				{
					token->rune = utf8char;
				} break;
			}

			if (next_utf8char(lexer, NULL, false) != '\'')
			{
				log_lexer_error(token->location, "expected trailing single quote.");
			}

			token->type = primec_token_type_literal_rune;
			return token->type;
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			//       that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
			return primec_token_type_none;
		} break;
	}

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(0); // Sanity check for developers.
	return primec_token_type_none;
}

static primec_token_type_e lex_string_literal_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	utf8char_t delimeter = primec_utf8_invalid;
	utf8char_t utf8char = next_utf8char(lexer, &token->location, false);
	char buffer[primec_utf8_max_size + 1];

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(
		utf8char != primec_utf8_invalid && '\"' == utf8char
	); // Sanity check for developers.

	switch (utf8char)
	{
		case '\"':
		{
			delimeter = utf8char;

			while ((utf8char = next_utf8char(lexer, NULL, false)) != delimeter)
			{
				if (utf8char == primec_utf8_invalid)
				{
					log_lexer_error(lexer->location, "unexpected end of file.");
				}

				push_utf8char(lexer, utf8char, false);

				if ('\"' == delimeter)
				{
					const uint8_t size = lex_possible_rune(lexer, buffer);
					append_buffer(lexer, buffer, size);
				}
				else
				{
					next_utf8char(lexer, NULL, true);
				}
			}

			char* const string = primec_utils_malloc(
				(lexer->buffer_length + 1) * sizeof(char));
			primec_utils_memcpy(string, lexer->buffer, lexer->buffer_length);

			token->type = primec_token_type_literal_str;
			token->str.data = string;
			token->str.length = lexer->buffer_length;

			clear_buffer(lexer);
			return token->type;
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			//       that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
			return primec_token_type_none;
		} break;
	}

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(0); // Sanity check for developers.
	return primec_token_type_none;
}

static primec_token_type_e lex_up_to_2_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	utf8char_t utf8char)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(utf8char != primec_utf8_invalid); // Sanity check for developers.

	switch (utf8char)
	{
		case '.':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '.':
				{
					token->type = primec_token_type_double_dot;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_dot;
					lexer->require_int = true;
				} break;
			}
		} break;

		case '*':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_multiply_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_star;
				} break;
			}
		} break;

		case '%':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_modulus_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_modulus;
				} break;
			}
		} break;

		case '/':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_divide_assign;
				} break;

				case '/':
				{
					token->type = primec_token_type_single_line_comment;
					while ((utf8char = next_utf8char(lexer, NULL, true)) != primec_utf8_invalid && utf8char != '\n');

					// NOTE: subtracting one from the length of the buffer to ignore the end of line symbol '\n':
					token->comment.data = primec_utils_strndup(lexer->buffer, lexer->buffer_length - 1);
					token->comment.length = lexer->buffer_length - 1;
					clear_buffer(lexer);
				} break;

				case '*':
				{
					token->type = primec_token_type_multi_line_comment;

					utf8char_t last_utf8char = utf8char;
					utf8char = next_utf8char(lexer, NULL, true);

					while (last_utf8char != primec_utf8_invalid && utf8char != primec_utf8_invalid
						&& (last_utf8char != '*' || utf8char != '/'))
					{
						last_utf8char = utf8char;
						utf8char = next_utf8char(lexer, NULL, true);
					}

					// NOTE: subtracting two from the length of the buffer to ignore the end of multi line comment
					//       symbols "*/":
					token->comment.data = primec_utils_strndup(lexer->buffer, lexer->buffer_length - 2);
					token->comment.length = lexer->buffer_length - 2;
					clear_buffer(lexer);
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_divide;
				} break;
			}
		} break;

		case '+':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_add_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_add;
				} break;
			}
		} break;

		case '-':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_subtract_assign;
				} break;

				case '>':
				{
					token->type = primec_token_type_arrow;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_subtract;
				} break;
			}
		} break;

		case ':':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case ':':
				{
					token->type = primec_token_type_double_colon;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_colon;
				} break;
			}
		} break;

		case '!':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_not_equal;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_lnot;
				} break;
			}
		} break;

		case '=':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_equal;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_assign;
				} break;
			}
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			//       that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
		} break;
	}

	return token->type;
}

static primec_token_type_e lex_up_to_3_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	uint32_t utf8char)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	primec_debug_assert(utf8char != primec_utf8_invalid); // Sanity check for developers.

	switch (utf8char)
	{
		case '<':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '<':
				{
					switch ((utf8char = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_lshift_assign;
						} break;

						default:
						{
							push_utf8char(lexer, utf8char, false);
							token->type = primec_token_type_lshift;
						} break;
					}
				} break;

				case '=':
				{
					token->type = primec_token_type_less_than_or_equal;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_less_than;
				} break;
			}
		} break;

		case '>':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '>':
				{
					switch ((utf8char = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_rshift_assign;
						} break;

						default:
						{
							push_utf8char(lexer, utf8char, false);
							token->type = primec_token_type_rshift;
						} break;
					}
				} break;

				case '=':
				{
					token->type = primec_token_type_greater_than_or_equal;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_greater_than;
				} break;
			}
		} break;

		case '&':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '&':
				{
					switch ((utf8char = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_land_assign;
						} break;

						default:
						{
							push_utf8char(lexer, utf8char, false);
							token->type = primec_token_type_land;
						} break;
					}
				} break;

				case '=':
				{
					token->type = primec_token_type_band_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_band;
				} break;
			}
		} break;

		case '|':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '|':
				{
					switch ((utf8char = next_utf8char(lexer, NULL, false)))
					{
					case '=':
						token->type = primec_token_type_lor_assign;
						break;

					default:
						push_utf8char(lexer, utf8char, false);
						token->type = primec_token_type_lor;
						break;
					}
				} break;

				case '=':
				{
					token->type = primec_token_type_bor_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_bor;
				} break;
			}
		} break;

		case '^':
		{
			switch ((utf8char = next_utf8char(lexer, NULL, false)))
			{
				case '^':
				{
					switch ((utf8char = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_lxor_assign;
						} break;

						default:
						{
							push_utf8char(lexer, utf8char, false);
							token->type = primec_token_type_lxor;
						} break;
					}
				} break;

				case '=':
				{
					token->type = primec_token_type_bxor_assign;
				} break;

				default:
				{
					push_utf8char(lexer, utf8char, false);
					token->type = primec_token_type_bxor;
				} break;
			}
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			//       that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
		} break;
	}

	return token->type;
}
