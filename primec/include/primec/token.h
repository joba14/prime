
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
	primec_token_let,					// let
	primec_token_mut,					// mut
	primec_token_ref,					// &
	primec_token_ptr,					// *
	primec_token_c8,					// c8
	primec_token_i8,					// i8
	primec_token_i16,					// i16
	primec_token_i32,					// i32
	primec_token_i64,					// i64
	primec_token_u8,					// u8
	primec_token_u16,					// u16
	primec_token_u32,					// u32
	primec_token_u64,					// u64
	primec_token_f32,					// f32
	primec_token_f64,					// f64
	primec_token_as,					// as
	primec_token_unsafe,				// unsafe
	primec_token_if,					// if
	primec_token_elif,					// elif
	primec_token_else,					// else
	primec_token_while,					// while
	primec_token_break,					// break
	primec_token_continue,				// continue
	primec_token_return,				// return
	primec_token_func,					// func
	primec_token_inl,					// inl
	primec_token_ext,					// ext
	primec_token_struct,				// struct
	primec_token_alias,					// alias

	primec_token_equal,					// =
	primec_token_add_equal,				// +=
	primec_token_subtract_equal,		// -=
	primec_token_multiply_equal,		// *=
	primec_token_divide_equal,			// /=
	primec_token_modulus_equal,			// %=

	primec_token_add,					// +
	primec_token_subtract,				// -
	primec_token_multiply,				// *
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

	primec_token_band,					// &
	primec_token_bor,					// |
	primec_token_bnot,					// ~
	primec_token_bxor,					// ^
	primec_token_lshift,				// <<
	primec_token_rshift,				// >>

	primec_token_left_parenth,			// (
	primec_token_right_parenth,			// )
	primec_token_left_bracket,			// [
	primec_token_right_bracket,			// ]
	primec_token_left_brace,			// {
	primec_token_right_brace,			// }
	primec_token_arrow,					// ->
	primec_token_semicolon,				// ;
	primec_token_colon,					// :
	primec_token_comma,					// ,
	primec_token_dot,					// .

	primec_token_single_line_comment,	// //
	primec_token_multi_line_comment,	// /*

	// Tokens with additional information
	primec_token_literal,
	primec_token_name,

	// Magic tokens
	primec_token_eof,
	primec_token_none
} primec_token_e;

typedef enum
{
	// Built-in types
	// The order of these is important
	primec_storage_b8,
	primec_storage_c8,
	primec_storage_i8,
	primec_storage_i16,
	primec_storage_i32,
	primec_storage_i64,
	primec_storage_u8,
	primec_storage_u16,
	primec_storage_u32,
	primec_storage_u64,
	primec_storage_f32,
	primec_storage_f64,

	// Other types
	STORAGE_ALIAS,
	STORAGE_ARRAY,
	STORAGE_ENUM,
	STORAGE_FUNCTION,
	STORAGE_POINTER,
	STORAGE_SLICE,
	STORAGE_STRUCT,
	STORAGE_TAGGED,
	STORAGE_TUPLE,
	STORAGE_UNION,
	STORAGE_VALIST,
	STORAGE_FCONST,
	STORAGE_ICONST,
	STORAGE_RCONST,
	// For internal use only
	STORAGE_ERROR,
} primec_storage_e;

typedef struct
{
	primec_location_s location;
	primec_token_e type;
	primec_storage_e storage;
	union
	{
		char* name;
		uint32_t rune;
		int64_t ival;
		uint64_t uval;
		long double fval;
		struct
		{
			uint64_t len;
			char* value;
		} string;
	};
} primec_token_s;

#endif
