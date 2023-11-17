
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

static char* utf8char_to_heap_string(
	const utf8char_t utf8char,
	uint64_t* const string_length)
{
	char* buffer = (char*)primec_utils_malloc(4 * sizeof(char));
	const uint8_t buffer_length = primec_utf8_encode(buffer, utf8char);
	primec_debug_assert(buffer_length <= 4);
	buffer = (char*)primec_utils_realloc(buffer, buffer_length);
	if (string_length != NULL) { *string_length = buffer_length; }
	return buffer;
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

	// NOTE: Should never ever happen as this function will get symbols
	// that are already verified to be correct ones!
	primec_debug_assert(utf8char != primec_utf8_invalid &&
		utf8char <= 0x7F && (isalpha(utf8char) || '_' == utf8char)
	);// Sanity check for developers.

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

static uint64_t compute_exponent(
	uint64_t n,
	const uint64_t exponent,
	const bool is_signed)
{
	if (n == 0)
	{
		return 0;
	}

	for (uint64_t index = 0; index < exponent; ++index)
	{
		const uint64_t old = n;
		n *= 10;

		if (n / 10 != old)
		{
			errno = ERANGE;
			return INT64_MAX;
		}
	}

	if (is_signed && n > (uint64_t)INT64_MIN)
	{
		errno = ERANGE;
		return INT64_MAX;
	}

	return n;
}

static void lex_any_literal(
	primec_lexer_s* const lexer,
	primec_token_s* const out)
{
	enum
	{
		base_bin = 1, base_oct, base_hex, base_dec = 0x07, base_mask = base_dec
	};

	_Static_assert((base_bin | base_oct | base_hex | base_dec) == base_dec, "base_dec bits must be a superset of all other bases");

	enum
	{
		flag_flt = 3, flag_exp, flag_suff, flag_dig
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
		['+'] = { base_dec | 1 << flag_exp | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_exp | 1 << flag_dig, 0 },
		['-'] = { base_dec | 1 << flag_exp | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_exp | 1 << flag_dig, 0 },
		['i'] = { base_bin, base_oct, base_hex, base_dec, base_dec | 1 << flag_exp, 0 },
		['u'] = { base_bin, base_oct, base_hex, base_dec, base_dec | 1 << flag_exp, 0 },
		['f'] = { base_dec, base_dec | 1 << flag_flt, base_dec | 1 << flag_exp, base_dec | 1 << flag_flt | 1 << flag_exp, 0 },
	};

	int32_t state = base_dec;
	int32_t base = 10;
	int32_t old_state = base_dec;

	utf8char_t c = next_utf8char(lexer, &out->location, true);
	utf8char_t last = 0;

	// TODO: make it not an assert?
	primec_debug_assert(c != primec_utf8_invalid && c <= 0x7F && (isdigit(c) || '+' == c || '-' == c));

	if (c == '0')
	{
		c = next_utf8char(lexer, NULL, true);

		if (c <= 0x7F && isdigit(c))
		{
			log_lexer_error(out->location, "leading zero in base 10 literal");
		}
		else if ('b' == c)
		{
			state = base_bin | 1 << flag_dig;
			base = 2;
		}
		else if ('o' == c)
		{
			state = base_oct | 1 << flag_dig;
			base = 8;
		}
		else if ('x' == c)
		{
			state = base_hex | 1 << flag_dig;
			base = 16;
		}
	}

	if (state != base_dec)
	{
		last = c;
		c = next_utf8char(lexer, NULL, true);
	}

	size_t exp = 0, suff = 0;

	do
	{
		if (strchr(chrs[state & base_mask], (int32_t)c))
		{
			state &= ~(1 << flag_dig);
			last = c;
			continue;
		}
		else if (c > 0x7f || !strchr(matching_states[c], state))
		{
			goto end;
		}

		old_state = state;

		switch (c)
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

			case 'e':
			case 'E':
			case '+':
			{
				state |= base_dec | 1 << flag_exp;
				exp = lexer->buffer_length - 1;
			} break;

			case 'f':
			{
				state |= 1 << flag_flt;
			} /* fallthrough */

			case 'i':
			case 'u':
			{
				state |= base_dec | 1 << suff;
				suff = lexer->buffer_length - 1;
			} break;

			default:
			{
				goto end;
			} break;
		}

		if (state & 1 << flag_flt && lexer->require_int)
		{
			log_lexer_error(out->location, "expected integer literal");
		}

		last = c;
		state |= 1 << flag_dig;
	} while ((c = next_utf8char(lexer, NULL, true)) != primec_utf8_invalid);

	last = 0;

end:
	if (last && !strchr("iu", (int32_t)last) && !strchr(chrs[state & base_mask], (int32_t)last))
	{
		state = old_state;
		push_utf8char(lexer, c, true);
		push_utf8char(lexer, last, true);
	}
	else if (c != primec_utf8_invalid)
	{
want_int:
		push_utf8char(lexer, c, true);
	}

	lexer->require_int = false;

	// TODO: remove (2):
	primec_logger_log("1");
	(void)getchar();

	typedef enum
	{
		kind_unknown = -1, kind_iconst, kind_signed, kind_unsigned, kind_float
	} kind_e;

	kind_e kind = kind_unknown;

	static const struct
	{
		const char suff[4];
		kind_e kind;
		primec_token_type_e type;
	} literals[] =
	{
		{ "f32", kind_float, primec_token_type_literal_f32 },
		{ "f64", kind_float, primec_token_type_literal_f64 },
		{ "i16", kind_signed, primec_token_type_literal_i16 },
		{ "i32", kind_signed, primec_token_type_literal_i32 },
		{ "i64", kind_signed, primec_token_type_literal_i64 },
		{ "i8", kind_signed, primec_token_type_literal_i8 },
		{ "u16", kind_unsigned, primec_token_type_literal_u16 },
		{ "u32", kind_unsigned, primec_token_type_literal_u32 },
		{ "u64", kind_unsigned, primec_token_type_literal_u64 },
		{ "u8", kind_unsigned, primec_token_type_literal_u8 }
	};

	if (suff)
	{
		for (uint8_t index = 0; index < (sizeof(literals) / sizeof(literals[0])); ++index)
		{
			if (!strcmp(literals[index].suff, lexer->buffer + suff))
			{
				out->type = literals[index].type;
				kind = literals[index].kind;
				break;
			}
		}

		if (kind == kind_unknown)
		{
			log_lexer_error(out->location, "invalid suffix '%s'", lexer->buffer + suff);
		}
	}

	// TODO: remove (2):
	primec_logger_log("2");
	(void)getchar();

	if (state & 1 << flag_flt)
	{
		if (kind_unknown == kind)
		{
			out->type = primec_token_type_literal_f64;
		}
		else if (kind != kind_float)
		{
			log_lexer_error(out->location, "unexpected decimal point in integer literal");
		}

		out->f64 = strtod(lexer->buffer, NULL);

		out->source.data = primec_utils_strdup(lexer->buffer);
		out->source.length = lexer->buffer_length;

		clear_buffer(lexer);
		return;
	}

	// TODO: remove (2):
	primec_logger_log("3");
	(void)getchar();

	if (kind == kind_unknown)
	{
		kind = kind_iconst;
		out->type = primec_token_type_literal_i64;
	}

	uint64_t exponent = 0;
	errno = 0;

	if (exp != 0)
	{
		exponent = strtoumax(lexer->buffer + exp + 1, NULL, 10);
	}

	out->u64 = strtoumax(lexer->buffer + (base == 10 ? 0 : 2), NULL, base);
	out->u64 = compute_exponent(out->u64, exponent, kind_signed == kind);

	if (ERANGE == errno)
	{
		log_lexer_error(out->location, "integer literal overflow");
	}

	if (kind_iconst == kind && out->u64 > (uint64_t)INT64_MAX)
	{
		out->type = primec_token_type_literal_u64;
	}
	else if (kind_signed == kind && out->u64 == (uint64_t)INT64_MIN)
	{
		out->i64 = INT64_MIN;
	}
	else if (kind != kind_unsigned)
	{
		out->i64 = (int64_t)out->u64;
	}

	// TODO: remove (2):
	primec_logger_log("4");
	primec_logger_log("%s", lexer->buffer);
	primec_logger_log("%.*s", (signed int)lexer->buffer_length, lexer->buffer);
	(void)getchar();

	out->source.data = primec_utils_strdup(lexer->buffer);
	out->source.length = lexer->buffer_length;
	clear_buffer(lexer);
}

static primec_token_type_e lex_up_to_2_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	utf8char_t c)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	// that are already verified to be correct ones!
	primec_debug_assert(c != primec_utf8_invalid); // Sanity check for developers.

	switch (c)
	{
		case '.':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '.':
				{
					token->type = primec_token_type_double_dot;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_dot;
					lexer->require_int = true;
				} break;
			}
		} break;

		case '*':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_multiply_assign;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_star;
				} break;
			}
		} break;

		case '%':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_modulus_assign;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_modulus;
				} break;
			}
		} break;

		case '/':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_divide_assign;
				} break;

				/*
				case '/':
				{
					token->type = primec_token_type_single_line_comment;

					// TODO: implement!
						while ((c = next_utf8char(lexer, NULL, false)) != primec_utf8_invalid && c != '\n');
						return lex(lexer, token);
				} return;
				*/

				/*
				case '*':
				{
					// TODO: implement!
				} return;
				*/

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_divide;
				} break;
			}
		} break;

		case '+':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_add_assign;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_add;
				} break;
			}
		} break;

		case '-':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_subtract;
				} break;
			}
		} break;

		case ':':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case ':':
				{
					token->type = primec_token_type_double_colon;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_colon;
				} break;
			}
		} break;

		case '!':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_not_equal;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_lnot;
				} break;
			}
		} break;

		case '=':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '=':
				{
					token->type = primec_token_type_less_than_or_equal;
				} break;

				default:
				{
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_equal;
				} break;
			}
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			// that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
		} break;
	}

	return token->type;
}

static primec_token_type_e lex_up_to_3_symbol_token(
	primec_lexer_s* const lexer,
	primec_token_s* const token,
	uint32_t c)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	// that are already verified to be correct ones!
	primec_debug_assert(c != primec_utf8_invalid); // Sanity check for developers.

	switch (c)
	{
		case '<':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '<':
				{
					switch ((c = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_lshift_assign;
						} break;

						default:
						{
							push_utf8char(lexer, c, false);
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_less_than;
				} break;
			}
		} break;

		case '>':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '>':
				{
					switch ((c = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_rshift_assign;
						} break;

						default:
						{
							push_utf8char(lexer, c, false);
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_greater_than;
				} break;
			}
		} break;

		case '&':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '&':
				{
					switch ((c = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_land_assign;
						} break;

						default:
						{
							push_utf8char(lexer, c, false);
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_band;
				} break;
			}
		} break;

		case '|':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '|':
				{
					switch ((c = next_utf8char(lexer, NULL, false)))
					{
					case '=':
						token->type = primec_token_type_lor_assign;
						break;

					default:
						push_utf8char(lexer, c, false);
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_bor;
				} break;
			}
		} break;

		case '^':
		{
			switch ((c = next_utf8char(lexer, NULL, false)))
			{
				case '^':
				{
					switch ((c = next_utf8char(lexer, NULL, false)))
					{
						case '=':
						{
							token->type = primec_token_type_lxor_assign;
						} break;

						default:
						{
							push_utf8char(lexer, c, false);
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
					push_utf8char(lexer, c, false);
					token->type = primec_token_type_bxor;
				} break;
			}
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			// that are already verified to be correct ones!
			primec_debug_assert(0); // Sanity check for developers.
		} break;
	}

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
	(void)memset((void*)lexer, 0, sizeof(primec_lexer_s));
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
		lexer->token.type = primec_token_type_eof;
		lexer->token.source.length = 0;
		*token = lexer->token;
		return token->type;
	}

	// TODO: uncomment/fix/implement numeric and other literals:
	/*
	if (c <= 0x7F && (isdigit(c) || '+' == c || '-' == c))
	{
		push_utf8char(lexer, c, false);
		lex_any_literal(lexer, token);
		return token->type;
	}

	lexer->require_int = false;
	*/

	// TODO: remove(4):
	if (0)
	{
		lex_any_literal(NULL, NULL);
	}

	if (c <= 0x7F && (isalpha(c) || '_' == c))
	{
		push_utf8char(lexer, c, false);
		return lex_identifier_or_keyword(lexer, token);
	}

	switch (c)
	{
// TODO: implement literals strings and chars:
/*
		case '"':
		case '`':
		case '\'':
		{
			push_utf8char(lexer, c, false);
			return lex_string(lexer, token);
		} break;
*/

		case '<': // < << <= <<=
		case '>': // > >> >= >>=
		case '&': // & && &= &&=
		case '|': // | || |= ||=
		case '^': // ^ ^^ ^= ^^=
		{
			return lex_up_to_3_symbol_token(lexer, token, c);
		} break;

		case '.': // . ..
		case '*': // * *=
		case '%': // % %=
		case '/': // / /= // /*
		case '+': // + +=
		case '-': // - -=
		case ':': // : ::
		case '!': // ! !=
		case '=': // = == =>
		{
			return lex_up_to_2_symbol_token(lexer, token, c);
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
			token->type = primec_token_type_invalid;
			token->source.data = utf8char_to_heap_string(c, &token->source.length);
			clear_buffer(lexer);
		} break;
	}

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
