
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
#include <primec/utf8.h>

#include <inttypes.h>

#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>

static const char* g_stringified_tokens[] =
{
	[primec_token_type_keyword_let] = "let",
	[primec_token_type_keyword_mut] = "mut",
	[primec_token_type_keyword_c8] = "c8",
	[primec_token_type_keyword_i8] = "i8",
	[primec_token_type_keyword_i16] = "i16",
	[primec_token_type_keyword_i32] = "i32",
	[primec_token_type_keyword_i64] = "i64",
	[primec_token_type_keyword_u8] = "u8",
	[primec_token_type_keyword_u16] = "u16",
	[primec_token_type_keyword_u32] = "u32",
	[primec_token_type_keyword_u64] = "u64",
	[primec_token_type_keyword_f32] = "f32",
	[primec_token_type_keyword_f64] = "f64",
	[primec_token_type_keyword_unsafe] = "unsafe",
	[primec_token_type_keyword_if] = "if",
	[primec_token_type_keyword_elif] = "elif",
	[primec_token_type_keyword_else] = "else",
	[primec_token_type_keyword_loop] = "loop",
	[primec_token_type_keyword_while] = "while",
	[primec_token_type_keyword_break] = "break",
	[primec_token_type_keyword_continue] = "continue",
	[primec_token_type_keyword_return] = "return",
	[primec_token_type_keyword_func] = "func",
	[primec_token_type_keyword_inl] = "inl",
	[primec_token_type_keyword_ext] = "ext",
	[primec_token_type_keyword_enum] = "enum",
	[primec_token_type_keyword_struct] = "struct",
	[primec_token_type_keyword_alias] = "alias",
	[primec_token_type_keyword_as] = "as",
	[primec_token_type_keyword_import] = "import",

	[primec_token_type_assign] = "=",
	[primec_token_type_add_assign] = "+=",
	[primec_token_type_subtract_assign] = "-=",
	[primec_token_type_multiply_assign] = "*=",
	[primec_token_type_divide_assign] = "/=",
	[primec_token_type_modulus_assign] = "%=",
	[primec_token_type_land_assign] = "&&=",
	[primec_token_type_lor_assign] = "||=",
	[primec_token_type_lxor_assign] = "^^=",
	[primec_token_type_band_assign] = "&=",
	[primec_token_type_bor_assign] = "|=",
	[primec_token_type_bnot_assign] = "~=",
	[primec_token_type_bxor_assign] = "^=",
	[primec_token_type_lshift_assign] = "<<=",
	[primec_token_type_rshift_assign] = ">>=",

	[primec_token_type_add] = "+",
	[primec_token_type_subtract] = "-",
	[primec_token_type_star] = "*",
	[primec_token_type_divide] = "/",
	[primec_token_type_modulus] = "%",

	[primec_token_type_equal] = "==",
	[primec_token_type_not_equal] = "!=",
	[primec_token_type_greater_than] = ">",
	[primec_token_type_less_than] = "<",
	[primec_token_type_greater_than_or_equal] = ">=",
	[primec_token_type_less_than_or_equal] = "<=",

	[primec_token_type_land] = "&&",
	[primec_token_type_lor] = "||",
	[primec_token_type_lnot] = "!",
	[primec_token_type_lxor] = "^^",

	[primec_token_type_ampersand] = "&",
	[primec_token_type_bor] = "|",
	[primec_token_type_bnot] = "~",
	[primec_token_type_bxor] = "^",
	[primec_token_type_lshift] = "<<",
	[primec_token_type_rshift] = ">>",

	[primec_token_type_left_parenth] = "(",
	[primec_token_type_right_parenth] = ")",
	[primec_token_type_left_bracket] = "[",
	[primec_token_type_right_bracket] = "]",
	[primec_token_type_left_brace] = "{",
	[primec_token_type_right_brace] = "}",
	[primec_token_type_arrow] = "->",
	[primec_token_type_semicolon] = ";",
	[primec_token_type_colon] = ":",
	[primec_token_type_double_colon] = "::",
	[primec_token_type_comma] = ",",
	[primec_token_type_dot] = ".",
	[primec_token_type_double_dot] = ".."
};

_Static_assert(
	(sizeof(g_stringified_tokens) / sizeof(const char*)) == (primec_token_type_informationless_count + 1),
	"g_stringified_tokens is not in sync with primec_token_type_e enum!"
);






#define primec_c_eof primec_utf8_invalid

static void update_location(
	primec_location_s* const location,
	const uint32_t c)
{
	primec_debug_assert(location != NULL);

	if ('\n' == c)
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

	if (lexer->buffer_length + size >= lexer->buffer_capacity)
	{
		lexer->buffer_capacity *= 2;
		lexer->buffer = primec_utils_realloc(lexer->buffer, lexer->buffer_capacity);
	}

	(void)memcpy(lexer->buffer + lexer->buffer_length, buffer, size);
	lexer->buffer_length += size;
	lexer->buffer[lexer->buffer_length] = 0;
}

static uint32_t next(
	primec_lexer_s* const lexer,
	primec_location_s* const location,
	const bool buffer)
{
	primec_debug_assert(lexer != NULL);
	uint32_t c;

	if (lexer->c[0] != UINT32_MAX)
	{
		c = lexer->c[0];
		lexer->c[0] = lexer->c[1];
		lexer->c[1] = UINT32_MAX;
	}
	else
	{
		c = primec_utf8_get(lexer->file);
		update_location(&lexer->location, c);

		if ((primec_utf8_invalid == c) && !feof(lexer->file))
		{
			primec_logger_panic(
				"invalid UTF-8 sequence encountered: " primec_location_fmt,
				primec_location_arg(lexer->location)
			);
		}
	}

	if (location != NULL)
	{
		*location = lexer->location;

		for (uint64_t i = 0; i < 2 && lexer->c[i] != UINT32_MAX; i++)
		{
			update_location(&lexer->location, lexer->c[i]);
		}
	}

	if ((primec_c_eof == c) || !buffer)
	{
		return c;
	}

	char buf[primec_utf8_max_size];
	const uint64_t size = primec_utf8_encode(&buf[0], c);
	append_buffer(lexer, buf, size);
	return c;
}

static bool is_white_space(
	const uint32_t c)
{
	return c == '\t' || c == '\n' || c == '\r' || c == ' ';
}

static uint32_t wgetc(
	primec_lexer_s* const lexer,
	primec_location_s* const location)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(location != NULL);

	uint32_t c;
	while ((c = next(lexer, location, false)) != primec_c_eof && is_white_space(c));
	return c;
}

static void clear_buffer(
	primec_lexer_s* const lexer)
{
	primec_debug_assert(lexer != NULL);
	lexer->buffer_length = 0;
	lexer->buffer[0] = 0;
}

static void consume(
	primec_lexer_s* const lexer,
	const uint64_t amount)
{
	primec_debug_assert(lexer != NULL);

	for (uint64_t index = 0; index < amount; ++index)
	{
		while ((lexer->buffer[--lexer->buffer_length] & 0xC0) == 0x80);
	}

	lexer->buffer[lexer->buffer_length] = 0;
}

static void push(
	primec_lexer_s* const lexer,
	const uint32_t c,
	const bool buffer)
{
	primec_debug_assert(lexer != NULL);

	primec_debug_assert(lexer->c[1] == UINT32_MAX);
	lexer->c[1] = lexer->c[0];
	lexer->c[0] = c;

	if (buffer)
	{
		consume(lexer, 1);
	}
}

static int32_t compare_keyword(
	const void* const left,
	const void* const right)
{
	primec_debug_assert(left != NULL);
	primec_debug_assert(right != NULL);
	return strcmp(*(const char**)left, *(const char**)right);
}

static primec_token_type_e lex_name(
	primec_lexer_s* const lexer,
	primec_token_s* const token)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(token != NULL);

	uint32_t c = next(lexer, &token->location, true);
	primec_debug_assert((c != primec_c_eof) && (c <= 0x7F) && (isalpha(c) || c == '_'));

	while ((c = next(lexer, NULL, true)) != primec_c_eof)
	{
		if (c > 0x7F || (!isalnum(c) && c != '_'))
		{
			push(lexer, c, true);
			break;
		}
	}

	void* found_token = bsearch(
		&lexer->buffer, g_stringified_tokens, primec_token_type_keywords_count + 1,
		sizeof(g_stringified_tokens[0]), compare_keyword
	);

	if (!found_token)
	{
		token->type = primec_token_type_identifier;
		token->source.data = primec_utils_strdup(lexer->buffer);
	}
	else
	{
		token->type = (const char**)found_token - g_stringified_tokens;
	}

	clear_buffer(lexer);
	return token->type;
}

static uint64_t compute_exponent(
	uint64_t n,
	const int32_t exponent,
	const bool _signed)
{
	if (n == 0)
	{
		return 0;
	}

	for (int32_t i = 0; i < exponent; ++i)
	{
		const uint64_t old = n;
		n *= 10;

		if (n / 10 != old)
		{
			// errno = ERANGE;
			return INT64_MAX;
		}
	}

	if (_signed && n > (uint64_t)INT64_MIN)
	{
		// errno = ERANGE;
		return INT64_MAX;
	}

	return n;
}

static void lex_literal(
	primec_lexer_s* const lexer,
	primec_token_s* const out)
{
	primec_debug_assert(lexer != NULL);
	primec_debug_assert(out != NULL);

	enum
	{
		BIN = 1, OCT, HEX, DEC = 0x07, MASK = DEC
	};

	_Static_assert((BIN | OCT | HEX | DEC) == DEC, "DEC bits must be a superset of all other bases");

	enum
	{
		FLT = 3, EXP, SUFF, DIG,
	};

	static const char chrs[][24] =
	{
		[BIN] = "01",
		[OCT] = "01234567",
		[DEC] = "0123456789",
		[HEX] = "0123456789abcdefABCDEF",
	};

	static const char matching_states[0x80][6] =
	{
		['.'] = { DEC, HEX, 0 },
		['e'] = { DEC, DEC | 1 << FLT, 0 },
		['E'] = { DEC, DEC | 1 << FLT, 0 },
		['p'] = { HEX, HEX | 1 << FLT, 0 },
		['P'] = { HEX, HEX | 1 << FLT, 0 },
		['+'] = { DEC | 1 << EXP | 1 << DIG, DEC | 1 << FLT | 1 << EXP | 1 << DIG, 0 },
		['-'] = { DEC | 1 << EXP | 1 << DIG, DEC | 1 << FLT | 1 << EXP | 1 << DIG, 0 },
		['i'] = { BIN, OCT, HEX, DEC, DEC | 1 << EXP, 0 },
		['u'] = { BIN, OCT, HEX, DEC, DEC | 1 << EXP, 0 },
		['z'] = { BIN, OCT, HEX, DEC, DEC | 1 << EXP, 0 },
		['f'] = { DEC, DEC | 1 << FLT, DEC | 1 << EXP, DEC | 1 << FLT | 1 << EXP, 0 },
	};

	int32_t state = DEC, base = 10, oldstate = DEC;
	uint32_t c = next(lexer, &out->location, true), last = 0;
	primec_debug_assert(c != primec_c_eof && c <= 0x7F && isdigit(c));

	if (c == '0')
	{
		c = next(lexer, NULL, true);

		if (c <= 0x7F && isdigit(c))
		{
			primec_logger_panic(primec_location_fmt ": leading zero in base 10 literal", primec_location_arg(out->location));
		}
		else if ('b' == c)
		{
			state = BIN | 1 << DIG;
			base = 2;
		}
		else if ('o' == c)
		{
			state = OCT | 1 << DIG;
			base = 8;
		}
		else if ('x' == c)
		{
			state = HEX | 1 << DIG;
			base = 16;
		}
	}

	if (state != DEC)
	{
		last = c;
		c = next(lexer, NULL, true);
	}

	size_t exp = 0, suff = 0;

	do
	{
		if (strchr(chrs[state & MASK], (int32_t)c))
		{
			state &= ~(1 << DIG);
			last = c;
			continue;
		}
		else if (c > 0x7f || !strchr(matching_states[c], state))
		{
			goto end;
		}

		oldstate = state;

		switch (c)
		{
			case '.':
			{
				if (lexer->require_int)
				{
					goto want_int;
				}

				state |= 1 << FLT;
			} break;

			case '-':
			case 'p':
			case 'P':
			{
				state |= 1 << FLT;
			} /* fallthrough */

			case 'e':
			case 'E':
			case '+':
			{
				state |= DEC | 1 << EXP;
				exp = lexer->buffer_length - 1;
			} break;

			case 'f':
			{
				state |= 1 << FLT;
			} /* fallthrough */

			case 'i':
			case 'u':
			case 'z':
			{
				state |= DEC | 1 << SUFF;
				suff = lexer->buffer_length - 1;
			} break;

			default:
			{
				goto end;
			} break;
		}

		if (state & 1 << FLT && lexer->require_int)
		{
			primec_logger_panic(primec_location_fmt ": expected integer literal", primec_location_arg(out->location));
		}

		last = c;
		state |= 1 << DIG;
	} while ((c = next(lexer, NULL, true)) != primec_c_eof);

	last = 0;

end:
	if (last && !strchr("iuz", (int32_t)last) && !strchr(chrs[state & MASK], (int32_t)last))
	{
		state = oldstate;
		push(lexer, c, true);
		push(lexer, last, true);
	}
	else if (c != primec_c_eof)
	{
want_int:
		push(lexer, c, true);
	}

	// out->type = T_LITERAL;
	lexer->require_int = false;

	typedef enum
	{
		UNKNOWN = -1,
		ICONST, SIGNED, UNSIGNED, FLOAT
	} kind_e;

	kind_e kind = UNKNOWN;

	static const struct
	{
		const char suff[4];
		kind_e kind;
		primec_token_type_e type;
	} literals[] =
	{
		{ "i8", SIGNED, primec_token_type_literal_i8 },
		{ "i16", SIGNED, primec_token_type_literal_i16 },
		{ "i32", SIGNED, primec_token_type_literal_i32 },
		{ "i64", SIGNED, primec_token_type_literal_i64 },
		{ "u8", SIGNED, primec_token_type_literal_u8 },
		{ "u16", SIGNED, primec_token_type_literal_u16 },
		{ "u32", SIGNED, primec_token_type_literal_u32 },
		{ "u64", SIGNED, primec_token_type_literal_u64 },
		{ "f32", FLOAT, primec_token_type_literal_f32 },
		{ "f64", FLOAT, primec_token_type_literal_f64 }
	};

	if (suff)
	{
		for (uint64_t i = 0; i < (sizeof(literals) / sizeof(literals[0])); i++)
		{
			if (!strcmp(literals[i].suff, lexer->buffer + suff))
			{
				out->type = literals[i].type;
				kind = literals[i].kind;
				break;
			}
		}

		if (kind == UNKNOWN)
		{
			primec_logger_panic(
				primec_location_fmt ": invalid suffix '%s'",
				primec_location_arg(out->location), lexer->buffer + suff
			);
		}
	}
	if (state & 1 << FLT)
	{
		if (kind == UNKNOWN)
		{
			out->type = primec_token_type_literal_f64;
		}
		else if (kind != FLOAT)
		{
			primec_logger_panic(
				primec_location_fmt ": unexpected decimal point in integer literal",
				primec_location_arg(out->location)
			);
		}

		out->f64 = strtod(lexer->buffer, NULL);
		clear_buffer(lexer);
		return;
	}

	if (kind == UNKNOWN)
	{
		kind = ICONST;
		out->type = primec_token_type_literal_i64;
	}

	int32_t exponent = 0;
	// errno = 0;

	if (exp != 0)
	{
		exponent = (int32_t)strtoumax(lexer->buffer + exp + 1, NULL, 10);
	}

	out->u64 = strtoumax(lexer->buffer + (base == 10 ? 0 : 2), NULL, base);
	out->u64 = compute_exponent(out->u64, exponent, kind == SIGNED);

	/*
	if (errno == ERANGE)
	{
		error(out->location, "Integer literal overflow");
	}
	*/

	if (kind == ICONST && out->u64 > (uint64_t)INT64_MAX)
	{
		out->type = primec_token_type_literal_u64;
	}
	else if (kind == SIGNED && out->u64 == (uint64_t)INT64_MIN)
	{
		// XXX: Hack
		out->i64 = INT64_MIN;
	}
	else if (kind != UNSIGNED)
	{
		out->i64 = (int64_t)out->u64;
	}

	clear_buffer(lexer);
}






static size_t lex_rune(
	primec_lexer_s* const lexer,
	char* out)
{
	char buf[9];
	char *endptr;
	primec_location_s loc;
	uint32_t c = next(lexer, NULL, false);
	primec_debug_assert(c != primec_c_eof);

	switch (c) {
	case '\\':
		loc = lexer->location;
		c = next(lexer, NULL, false);
		switch (c) {
		case '0':
			out[0] = '\0';
			return 1;
		case 'a':
			out[0] = '\a';
			return 1;
		case 'b':
			out[0] = '\b';
			return 1;
		case 'f':
			out[0] = '\f';
			return 1;
		case 'n':
			out[0] = '\n';
			return 1;
		case 'r':
			out[0] = '\r';
			return 1;
		case 't':
			out[0] = '\t';
			return 1;
		case 'v':
			out[0] = '\v';
			return 1;
		case '\\':
			out[0] = '\\';
			return 1;
		case '\'':
			out[0] = '\'';
			return 1;
		case '"':
			out[0] = '\"';
			return 1;
		case 'x':
			buf[0] = (char)next(lexer, NULL, false);
			buf[1] = (char)next(lexer, NULL, false);
			buf[2] = '\0';
			c = (uint32_t)strtoul(&buf[0], &endptr, 16);
			if (*endptr != '\0') {
				primec_logger_panic(
					primec_location_fmt ": invalid hex literal",
					primec_location_arg(loc)
				);
			}
			out[0] = (char)c;
			return 1;
		case 'u':
			buf[0] = (char)next(lexer, NULL, false);
			buf[1] = (char)next(lexer, NULL, false);
			buf[2] = (char)next(lexer, NULL, false);
			buf[3] = (char)next(lexer, NULL, false);
			buf[4] = '\0';
			c = (uint32_t)strtoul(&buf[0], &endptr, 16);
			if (*endptr != '\0') {
				primec_logger_panic(
					primec_location_fmt ": invalid hex literal",
					primec_location_arg(loc)
				);
			}
			return primec_utf8_encode(out, c);
		case 'U':
			buf[0] = (char)next(lexer, NULL, false);
			buf[1] = (char)next(lexer, NULL, false);
			buf[2] = (char)next(lexer, NULL, false);
			buf[3] = (char)next(lexer, NULL, false);
			buf[4] = (char)next(lexer, NULL, false);
			buf[5] = (char)next(lexer, NULL, false);
			buf[6] = (char)next(lexer, NULL, false);
			buf[7] = (char)next(lexer, NULL, false);
			buf[8] = '\0';
			c = (uint32_t)strtoul(&buf[0], &endptr, 16);
			if (*endptr != '\0') {
				primec_logger_panic(
					primec_location_fmt ": invalid hex literal",
					primec_location_arg(loc)
				);
			}
			return primec_utf8_encode(out, c);
		case primec_c_eof:
			primec_logger_panic(
				primec_location_fmt ": unexpected end of file",
				primec_location_arg(lexer->location)
			);
			break;
		default:
			primec_logger_panic(
				primec_location_fmt ": invalid escape",
				primec_location_arg(loc)
			);
			break;
		}
		primec_debug_assert(0);
		break;
	default:
		return primec_utf8_encode(out, c);
	}

	primec_debug_assert(0);
	return primec_token_type_none;
}

static primec_token_type_e lex_string(
	primec_lexer_s* const lexer,
	primec_token_s* const out)
{
	uint32_t c = next(lexer, &out->location, false);
	uint32_t delim;
	char buf[primec_utf8_max_size + 1];

	switch (c) {
	case '"':
	case '`':
		delim = c;
		while ((c = next(lexer, NULL, false)) != delim) {
			if (c == primec_c_eof) {
				primec_logger_panic(primec_location_fmt ": unexpected end of file",
					primec_location_arg(lexer->location)
				);
			}
			push(lexer, c, false);
			if (delim == '"') {
				size_t sz = lex_rune(lexer, buf);
				append_buffer(lexer, buf, sz);
			} else {
				(void)next(lexer, NULL, true);
			}
		}
		char *s = primec_utils_malloc((lexer->buffer_length + 1) * sizeof(char));
		(void)memcpy(s, lexer->buffer, lexer->buffer_length);
		out->type = primec_token_type_literal_str;
		out->source.length = lexer->buffer_length;
		out->source.data = s;
		clear_buffer(lexer);
		return out->type;
	/*
	case '\'':
		c = (char)next(lexer, NULL, false);
		switch (c) {
		case '\'':
			error(out->location, "Expected rune before trailing single quote");
		case '\\':
			push(lexer, c, false);
			primec_location_s loc = lexer->location;
			size_t sz = lex_rune(lexer, buf);
			buf[sz] = '\0';
			const char *s = buf;
			out->rune = utf8_decode(&s);
			if (out->rune == primec_utf8_invalid) {
				error(loc, "invalid UTF-8 in rune literal");
			}
			break;
		default:
			out->rune = c;
		}
		if (next(lexer, NULL, false) != '\'') {
			error(out->location, "Expected trailing single quote");
		}
		out->type = T_LITERAL;
		out->storage = STORAGE_RCONST;
		return out->type;
	*/
	default:
		primec_debug_assert(0); // Invariant
	}

	primec_debug_assert(0);
	return primec_token_type_none;
}

static primec_token_type_e lex3(
	primec_lexer_s* const lexer,
	primec_token_s* const out,
	uint32_t c)
{
	primec_debug_assert(c != primec_c_eof);

	switch (c)
	{
	case '<':
		switch ((c = next(lexer, NULL, false))) {
		case '<':
			switch ((c = next(lexer, NULL, false))) {
			case '=':
				out->type = primec_token_type_lshift_assign;
				break;
			default:
				push(lexer, c, false);
				out->type = primec_token_type_lshift;
				break;
			}
			break;
		case '=':
			out->type = primec_token_type_less_than_or_equal;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_less_than;
			break;
		}
		break;
	case '>':
		switch ((c = next(lexer, NULL, false))) {
		case '>':
			switch ((c = next(lexer, NULL, false))) {
			case '=':
				out->type = primec_token_type_rshift_assign;
				break;
			default:
				push(lexer, c, false);
				out->type = primec_token_type_rshift;
				break;
			}
			break;
		case '=':
			out->type = primec_token_type_greater_than_or_equal;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_greater_than;
			break;
		}
		break;
	case '&':
		switch ((c = next(lexer, NULL, false))) {
		case '&':
			switch ((c = next(lexer, NULL, false))) {
			case '=':
				out->type = primec_token_type_land_assign;
				break;
			default:
				push(lexer, c, false);
				out->type = primec_token_type_land;
				break;
			}
			break;
		case '=':
			out->type = primec_token_type_band_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_ampersand;
			break;
		}
		break;
	case '|':
		switch ((c = next(lexer, NULL, false))) {
		case '|':
			switch ((c = next(lexer, NULL, false))) {
			case '=':
				out->type = primec_token_type_lor_assign;
				break;
			default:
				push(lexer, c, false);
				out->type = primec_token_type_lor;
				break;
			}
			break;
		case '=':
			out->type = primec_token_type_bor_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_bor;
			break;
		}
		break;
	case '^':
		switch ((c = next(lexer, NULL, false))) {
		case '^':
			switch ((c = next(lexer, NULL, false))) {
			case '=':
				out->type = primec_token_type_lxor_assign;
				break;
			default:
				push(lexer, c, false);
				out->type = primec_token_type_lxor;
				break;
			}
			break;
		case '=':
			out->type = primec_token_type_bxor_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_bxor;
			break;
		}
		break;
	default:
		primec_debug_assert(0); // Invariant
	}

	return out->type;
}

static primec_token_type_e lex2(
	primec_lexer_s* const lexer,
	primec_token_s* const out,
	uint32_t c)
{
	primec_debug_assert(c != primec_c_eof);

	switch (c) {
	case '*':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_multiply_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_star;
			break;
		}
		break;
	case '%':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_modulus_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_modulus;
			break;
		}
		break;
	case '/':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_divide_assign;
			break;
		case '/':
			while ((c = next(lexer, NULL, false)) != primec_c_eof && c != '\n') ;
			return primec_lexer_lex(lexer, out);
		default:
			push(lexer, c, false);
			out->type = primec_token_type_divide;
			break;
		}
		break;
	case '+':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_add_assign;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_add;
			break;
		}
		break;
	case '-':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_subtract_assign;
			break;
		case '>':
			out->type = primec_token_type_arrow;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_subtract;
			break;
		}
		break;
	case ':':
		switch ((c = next(lexer, NULL, false))) {
		case ':':
			out->type = primec_token_type_double_colon;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_colon;
			break;
		}
		break;
	case '.':
		switch ((c = next(lexer, NULL, false))) {
		case '.':
			out->type = primec_token_type_double_dot;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_dot;
			break;
		}
		break;
	case '!':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_not_equal;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_lnot;
			break;
		}
		break;
	case '=':
		switch ((c = next(lexer, NULL, false))) {
		case '=':
			out->type = primec_token_type_equal;
			break;
		default:
			push(lexer, c, false);
			out->type = primec_token_type_equal;
			break;
		}
		break;
	default:
		primec_debug_assert(0); // Invariant
	}

	return out->type;
}

static const char *
rune_unparse(uint32_t c)
{
	static char buf[11];
	switch (c) {
	case '\0':
		snprintf(buf, sizeof(buf), "\\0");
		break;
	case '\a':
		snprintf(buf, sizeof(buf), "\\a");
		break;
	case '\b':
		snprintf(buf, sizeof(buf), "\\b");
		break;
	case '\f':
		snprintf(buf, sizeof(buf), "\\f");
		break;
	case '\n':
		snprintf(buf, sizeof(buf), "\\n");
		break;
	case '\r':
		snprintf(buf, sizeof(buf), "\\r");
		break;
	case '\t':
		snprintf(buf, sizeof(buf), "\\t");
		break;
	case '\v':
		snprintf(buf, sizeof(buf), "\\v");
		break;
	case '\\':
		snprintf(buf, sizeof(buf), "\\\\");
		break;
	case '\'':
		snprintf(buf, sizeof(buf), "\\'");
		break;
	case '"':
		snprintf(buf, sizeof(buf), "\\\"");
		break;
	default:
		if (c > 0xffff) {
			snprintf(buf, sizeof(buf), "\\U%08x", c);
		} else if (c > 0x7F) {
			snprintf(buf, sizeof(buf), "\\u%04x", c);
		} else if (!isprint(c)) {
			snprintf(buf, sizeof(buf), "\\x%02x", c);
		} else {
			buf[primec_utf8_encode(buf, c)] = '\0';
		}
		break;
	}
	return buf;
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
	lexer.c[0] = UINT32_MAX;
	lexer.c[1] = UINT32_MAX;
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

	uint32_t c = wgetc(lexer, &token->location);

	if (c == primec_c_eof)
	{
		token->type = primec_token_type_eof;
		return token->type;
	}

	if (c <= 0x7F && isdigit(c)) {
		push(lexer, c, false);
		lex_literal(lexer, token);
		return token->type;
	}

	lexer->require_int = false;

	if (c <= 0x7F && (isalpha(c) || c == '_')) {
		push(lexer, c, false);
		return lex_name(lexer, token);
	}

	switch (c) {
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
		return lex3(lexer, token, c);
	case '*': // * *=
	case '%': // % %=
	case '/': // / /= //
	case '+': // + +=
	case '-': // - -=
	case ':': // : ::
	case '!': // ! !=
	case '=': // = == =>
		return lex2(lexer, token, c);
	case '~':
		token->type = primec_token_type_bnot;
		break;
	case ',':
		token->type = primec_token_type_comma;
		break;
	case '{':
		token->type = primec_token_type_left_brace;
		break;
	case '[':
		token->type = primec_token_type_left_bracket;
		break;
	case '(':
		token->type = primec_token_type_left_parenth;
		break;
	case '}':
		token->type = primec_token_type_right_brace;
		break;
	case ']':
		token->type = primec_token_type_right_bracket;
		break;
	case ')':
		token->type = primec_token_type_right_parenth;
		break;
	case ';':
		token->type = primec_token_type_semicolon;
		break;
	default:
		primec_logger_panic("%s:%lu:%lu: syntax error: unexpected codepoint '%s'\n",
			lexer->location.file, lexer->location.line,
			lexer->location.column, rune_unparse(c));
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
