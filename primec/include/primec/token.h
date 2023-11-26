
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

#include <primec/location.h>
#include <primec/utf8.h>

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	primec_token_type_keyword_alias,			// alias
	primec_token_type_keyword_as,				// as
	primec_token_type_keyword_break,			// break
	primec_token_type_keyword_c8,				// c8
	primec_token_type_keyword_continue,			// continue
	primec_token_type_keyword_elif,				// elif
	primec_token_type_keyword_else,				// else
	primec_token_type_keyword_enum,				// enum
	primec_token_type_keyword_ext,				// ext
	primec_token_type_keyword_f32,				// f32
	primec_token_type_keyword_f64,				// f64
	primec_token_type_keyword_func,				// func
	primec_token_type_keyword_i16,				// i16
	primec_token_type_keyword_i32,				// i32
	primec_token_type_keyword_i64,				// i64
	primec_token_type_keyword_i8,				// i8
	primec_token_type_keyword_if,				// if
	primec_token_type_keyword_inl,				// inl
	primec_token_type_keyword_let,				// let
	primec_token_type_keyword_loop,				// loop
	primec_token_type_keyword_mut,				// mut
	primec_token_type_keyword_return,			// return
	primec_token_type_keyword_struct,			// struct
	primec_token_type_keyword_u16,				// u16
	primec_token_type_keyword_u32,				// u32
	primec_token_type_keyword_u64,				// u64
	primec_token_type_keyword_u8,				// u8
	primec_token_type_keyword_unsafe,			// unsafe
	primec_token_type_keyword_use,				// use
	primec_token_type_keyword_while,			// while
	primec_token_type_keywords_count = primec_token_type_keyword_while,

	primec_token_type_assign,					// =
	primec_token_type_add_assign,				// +=
	primec_token_type_subtract_assign,			// -=
	primec_token_type_multiply_assign,			// *=
	primec_token_type_divide_assign,			// /=
	primec_token_type_modulus_assign,			// %=
	primec_token_type_land_assign,				// &&=
	primec_token_type_lor_assign,				// ||=
	primec_token_type_lxor_assign,				// ^^=
	primec_token_type_band_assign,				// &=
	primec_token_type_bor_assign,				// |=
	primec_token_type_bnot_assign,				// ~=
	primec_token_type_bxor_assign,				// ^=
	primec_token_type_lshift_assign,			// <<=
	primec_token_type_rshift_assign,			// >>=

	primec_token_type_add,						// +
	primec_token_type_subtract,					// -
	primec_token_type_star,						// *
	primec_token_type_pointer = primec_token_type_star,		// *
	primec_token_type_multiply = primec_token_type_star,	// *
	primec_token_type_divide,					// /
	primec_token_type_modulus,					// %

	primec_token_type_equal,					// ==
	primec_token_type_not_equal,				// !=
	primec_token_type_greater_than,				// >
	primec_token_type_less_than,				// <
	primec_token_type_greater_than_or_equal,	// >=
	primec_token_type_less_than_or_equal,		// <=

	primec_token_type_land,						// &&
	primec_token_type_lor,						// ||
	primec_token_type_lnot,						// !
	primec_token_type_lxor,						// ^^

	primec_token_type_ampersand,				// &
	primec_token_type_reference = primec_token_type_ampersand,	// &
	primec_token_type_band = primec_token_type_ampersand,		// &
	primec_token_type_bor,						// |
	primec_token_type_bnot,						// ~
	primec_token_type_bxor,						// ^
	primec_token_type_lshift,					// <<
	primec_token_type_rshift,					// >>

	primec_token_type_left_parenth,				// (
	primec_token_type_right_parenth,			// )
	primec_token_type_left_bracket,				// [
	primec_token_type_right_bracket,			// ]
	primec_token_type_left_brace,				// {
	primec_token_type_right_brace,				// }
	primec_token_type_arrow,					// ->
	primec_token_type_semicolon,				// ;
	primec_token_type_colon,					// :
	primec_token_type_double_colon,				// ::
	primec_token_type_comma,					// ,
	primec_token_type_dot,						// .
	primec_token_type_slice,					// ..
	primec_token_type_ellipsis,					// ...
	primec_token_type_informationless_count = primec_token_type_ellipsis,

	// Tokens with additional information
	primec_token_type_single_line_comment,		// //
	primec_token_type_multi_line_comment,		// /**/

	primec_token_type_literal_rune,
	primec_token_type_literal_i8,
	primec_token_type_literal_i16,
	primec_token_type_literal_i32,
	primec_token_type_literal_i64,
	primec_token_type_literal_u8,
	primec_token_type_literal_u16,
	primec_token_type_literal_u32,
	primec_token_type_literal_u64,
	primec_token_type_literal_f32,
	primec_token_type_literal_f64,
	primec_token_type_literal_str,

	primec_token_type_identifier,
	primec_token_type_invalid,

	// Magic tokens
	primec_token_type_eof,
	primec_token_type_none
} primec_token_type_e;

/**
 * @brief Try to parse token type from a string.
 */
primec_token_type_e primec_token_type_from_string(
	const char* const string);

/**
 * @brief Stringify token type.
 */
const char* primec_token_type_to_string(
	const primec_token_type_e type);

typedef struct
{
	primec_token_type_e type;
	primec_location_s location;

	union
	{
		struct
		{
			char* data;
			uint64_t length;
		} comment;

		utf8char_t rune;
		int64_t ival;
		uint64_t uval;
		long double fval;

		struct
		{
			char* data;
			uint64_t length;
		} str;
		struct
		{
			char* data;
			uint64_t length;
		} ident;
		struct
		{
			char* data;
			uint64_t length;
		} invalid;
	};
} primec_token_s;

/**
 * @brief Create token with provided token type and location.
 * 
 * @note All the other fields will be set to 0.
 */
primec_token_s primec_token_from_parts(
	const primec_token_type_e type,
	const primec_location_s location);

/**
 * @brief Create token with provided token type.
 * 
 * @note All the other fields will be set to 0.
 */
primec_token_s primec_token_from_type(
	const primec_token_type_e type);

/**
 * @brief Destroy token and free all its resources.
 */
void primec_token_destroy(
	primec_token_s* const token);

/**
 * @brief Convert token structure into a string representation and return a
 * pointer to it.
 * 
 * @warning The formatted token string is safed in the static buffer that gets
 * overwritten every time this function is called!
 */
const char* primec_token_to_string(
	const primec_token_s* const token);

#endif
