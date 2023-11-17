
/**
 * @file token.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-12
 */

#include <primec/token.h>

#include <primec/debug.h>
#include <primec/logger.h>
#include <primec/logger.h>
#include <primec/utils.h>

#include <stddef.h>
#include <stdio.h>

static const char* g_token_type_to_string_map[] =
{
	[primec_token_type_keyword_alias] = "alias",
	[primec_token_type_keyword_as] = "as",
	[primec_token_type_keyword_break] = "break",
	[primec_token_type_keyword_c8] = "c8",
	[primec_token_type_keyword_continue] = "continue",
	[primec_token_type_keyword_elif] = "elif",
	[primec_token_type_keyword_else] = "else",
	[primec_token_type_keyword_enum] = "enum",
	[primec_token_type_keyword_ext] = "ext",
	[primec_token_type_keyword_f32] = "f32",
	[primec_token_type_keyword_f64] = "f64",
	[primec_token_type_keyword_func] = "func",
	[primec_token_type_keyword_i16] = "i16",
	[primec_token_type_keyword_i32] = "i32",
	[primec_token_type_keyword_i64] = "i64",
	[primec_token_type_keyword_i8] = "i8",
	[primec_token_type_keyword_if] = "if",
	[primec_token_type_keyword_import] = "import",
	[primec_token_type_keyword_inl] = "inl",
	[primec_token_type_keyword_let] = "let",
	[primec_token_type_keyword_loop] = "loop",
	[primec_token_type_keyword_mut] = "mut",
	[primec_token_type_keyword_return] = "return",
	[primec_token_type_keyword_struct] = "struct",
	[primec_token_type_keyword_u16] = "u16",
	[primec_token_type_keyword_u32] = "u32",
	[primec_token_type_keyword_u64] = "u64",
	[primec_token_type_keyword_u8] = "u8",
	[primec_token_type_keyword_unsafe] = "unsafe",
	[primec_token_type_keyword_while] = "while",

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
	(sizeof(g_token_type_to_string_map) / sizeof(const char*)) == (primec_token_type_informationless_count + 1),
	"g_token_type_to_string_map is not in sync with primec_token_type_e enum!"
);

static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right);

primec_token_type_e primec_token_type_from_string(
	const char* const string)
{
	const void* const found_token = (const void* const)primec_utils_bsearch(
		&string, g_token_type_to_string_map, primec_token_type_keywords_count + 1,
		sizeof(g_token_type_to_string_map[0]), compare_keyword_tokens
	);

	return !found_token ? primec_token_type_identifier :
		(primec_token_type_e)((const char**)found_token - g_token_type_to_string_map);
}

const char* primec_token_type_to_string(
	const primec_token_type_e type)
{
	switch (type)
	{
		case primec_token_type_identifier:
		{
			return "identifier";
		} break;

		case primec_token_type_literal_c8:
		{
			return "literal_c8";
		} break;

		case primec_token_type_literal_i8:
		{
			return "literal_i8";
		} break;

		case primec_token_type_literal_i16:
		{
			return "literal_i16";
		} break;

		case primec_token_type_literal_i32:
		{
			return "literal_i32";
		} break;

		case primec_token_type_literal_i64:
		{
			return "literal_i64";
		} break;

		case primec_token_type_literal_u8:
		{
			return "literal_u8";
		} break;

		case primec_token_type_literal_u16:
		{
			return "literal_u16";
		} break;

		case primec_token_type_literal_u32:
		{
			return "literal_u32";
		} break;

		case primec_token_type_literal_u64:
		{
			return "literal_u64";
		} break;

		case primec_token_type_literal_f32:
		{
			return "literal_f32";
		} break;

		case primec_token_type_literal_f64:
		{
			return "literal_f64";
		} break;

		case primec_token_type_literal_str:
		{
			return "literal_str";
		} break;

		case primec_token_type_invalid:
		{
			return "invalid";
		} break;

		case primec_token_type_eof:
		{
			return "eof";
		} break;

		case primec_token_type_none:
		{
			return "none";
		} break;

		default:
		{
			primec_debug_assert(type < (sizeof(g_token_type_to_string_map) / sizeof(g_token_type_to_string_map[0])));
			const char* const stringified_type = (const char* const)g_token_type_to_string_map[type];
			primec_debug_assert(stringified_type != NULL);
			return stringified_type;
		} break;
	}
}

primec_token_s primec_token_from_parts(
	const primec_token_type_e type,
	const primec_location_s location)
{
	primec_token_s token;
	primec_utils_memset(
		(void* const)&token, 0, sizeof(primec_token_s)
	);

	token.type = type;
	token.location = location;
	return token;
}

primec_token_s primec_token_from_type(
	const primec_token_type_e type)
{
	primec_token_s token;
	primec_utils_memset(
		(void* const)&token, 0, sizeof(primec_token_s)
	);

	token.type = type;
	return token;
}

void primec_token_destroy(
	primec_token_s* const token)
{
	primec_debug_assert(token != NULL);
	primec_utils_free(token->source.data);
	primec_utils_memset(
		(void* const)&token, 0, sizeof(primec_token_s)
	);
}

const char* primec_token_to_string(
	const primec_token_s* const token)
{
	primec_debug_assert(token != NULL);
	#define token_string_buffer_capacity 1024
	static char token_string_buffer[token_string_buffer_capacity + 1];
	token_string_buffer[0] = 0;

	const uint64_t written = (uint64_t)snprintf(
		token_string_buffer, token_string_buffer_capacity,
		"Token[type=`%s`, location=`" primec_location_fmt "`, source=`%.*s`]",
		primec_token_type_to_string(token->type),
		primec_location_arg(token->location),
		(signed int)token->source.length,
		token->source.data
	);

	token_string_buffer[written] = 0;
	#undef token_string_buffer_capacity
	return token_string_buffer;
}

static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right)
{
	return primec_utils_strcmp(
		*(const char**)left, *(const char**)right
	);
}
