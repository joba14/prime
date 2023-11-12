
/**
 * @file token.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-12
 */

#ifndef __primec__include__primec__token_h__
#define __primec__include__primec__token_h__

#include <stdint.h>

typedef struct
{
	const char* file;
	uint64_t line;
	uint64_t column;
} primec_location_s;

typedef enum
{
	primec_token_keyword_let,			// let
	primec_token_keyword_mut,			// mut
	primec_token_keyword_c8,			// c8
	primec_token_keyword_i8,			// i8
	primec_token_keyword_i16,			// i16
	primec_token_keyword_i32,			// i32
	primec_token_keyword_i64,			// i64
	primec_token_keyword_u8,			// u8
	primec_token_keyword_u16,			// u16
	primec_token_keyword_u32,			// u32
	primec_token_keyword_u64,			// u64
	primec_token_keyword_f32,			// f32
	primec_token_keyword_f64,			// f64
	primec_token_keyword_unsafe,		// unsafe
	primec_token_keyword_if,			// if
	primec_token_keyword_elif,			// elif
	primec_token_keyword_else,			// else
	primec_token_keyword_loop,			// loop
	primec_token_keyword_while,			// while
	primec_token_keyword_break,			// break
	primec_token_keyword_continue,		// continue
	primec_token_keyword_return,		// return
	primec_token_keyword_func,			// func
	primec_token_keyword_inl,			// inl
	primec_token_keyword_ext,			// ext
	primec_token_keyword_struct,		// struct
	primec_token_keyword_alias,			// alias
	primec_token_keyword_as,			// as
	primec_token_keyword_import,		// import

	primec_token_assign,				// =
	primec_token_add_assign,			// +=
	primec_token_subtract_assign,		// -=
	primec_token_multiply_assign,		// *=
	primec_token_divide_assign,			// /=
	primec_token_modulus_assign,		// %=
	primec_token_land_assign,			// &&=
	primec_token_lor_assign,			// ||=
	primec_token_band_assign,			// &=
	primec_token_bor_assign,			// |=
	primec_token_bnot_assign,			// ~=
	primec_token_bxor_assign,			// ^=
	primec_token_lshift_assign,			// <<=
	primec_token_rshift_assign,			// >>=

	primec_token_add,					// +
	primec_token_subtract,				// -
	primec_token_star,					// *
	primec_token_pointer = primec_token_star,	// *
	primec_token_multiply = primec_token_star,	// *
	primec_token_divide,				// /
	primec_token_modulus,				// %

	primec_token_equal,					// ==
	primec_token_not_equal,				// !=
	primec_token_greater_than,			// >
	primec_token_less_than,				// <
	primec_token_greater_than_or_equal,	// >=
	primec_token_less_than_or_equal,	// <=

	primec_token_land,					// &&
	primec_token_lor,					// ||
	primec_token_lnot,					// !

	primec_token_ampersand,				// &
	primec_token_reference = primec_token_ampersand,	// &
	primec_token_band = primec_token_ampersand,	// &
	primec_token_bor,					// |
	primec_token_bnot,					// ~
	primec_token_bxor,					// ^
	primec_token_lshift,				// <<
	primec_token_rshift,				// >>

	primec_token_double_quote,			// "
	primec_token_single_quote,			// '
	primec_token_left_parenth,			// (
	primec_token_right_parenth,			// )
	primec_token_left_bracket,			// [
	primec_token_right_bracket,			// ]
	primec_token_left_brace,			// {
	primec_token_right_brace,			// }
	primec_token_arrow,					// ->
	primec_token_semicolon,				// ;
	primec_token_colon,					// :
	primec_token_double_colon,			// ::
	primec_token_comma,					// ,
	primec_token_dot,					// .

	primec_token_single_line_comment,	// //
	primec_token_multi_line_comment,	// /**/

	// Tokens with additional information
	primec_token_literal_c8,
	primec_token_literal_i8,
	primec_token_literal_i16,
	primec_token_literal_i32,
	primec_token_literal_i64,
	primec_token_literal_u8,
	primec_token_literal_u16,
	primec_token_literal_u32,
	primec_token_literal_u64,
	primec_token_literal_f32,
	primec_token_literal_f64,
	primec_token_literal_str,

	primec_token_name,

	// Magic tokens
	primec_token_eof,
	primec_token_none
} primec_token_e;

typedef struct
{
	primec_location_s location;
	primec_token_e type;

	union
	{
		char c8;
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;
		float f32;
		long double f64;

		struct
		{
			char* data;
			uint64_t length;
		} str;

		struct
		{
			char* data;
			uint64_t length;
		} name;
	};
} primec_token_s;

#endif
