
/**
 * @file tokenizer.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#ifndef __primec__include__tokenizer_h__
#define __primec__include__tokenizer_h__

#include <string_view.h>
#include <linked_list.h>

#include <stdint.h>

typedef struct
{
	string_view_s file;
	uint64_t line;
	uint64_t column;
} location_s;

typedef enum
{
	token_invalid = 0,
	token_identifier,

	token_literal_c8,
	token_literal_i8,
	token_literal_i16,
	token_literal_i32,
	token_literal_i64,
	token_literal_u8,
	token_literal_u16,
	token_literal_u32,
	token_literal_u64,
	token_literal_f32,
	token_literal_f64,
	token_literal_str,

	// TODO: reference and dereference operators!

	token_keyword_let,							// let
	token_keyword_mut,							// mut
	token_keyword_unsafe,						// unsafe
	token_keyword_if,							// if
	token_keyword_elif,							// elif
	token_keyword_else,							// else
	token_keyword_while,						// while
	token_keyword_break,						// break
	token_keyword_continue,						// continue
	token_keyword_return,						// return
	token_keyword_func,							// func
	token_keyword_inl,							// inl
	token_keyword_ext,							// ext
	token_keyword_struct,						// struct
	token_keyword_alias,						// alias
	token_keyword_c8,							// c8
	token_keyword_i8,							// i8
	token_keyword_i16,							// i16
	token_keyword_i32,							// i32
	token_keyword_i64,							// i64
	token_keyword_u8,							// u8
	token_keyword_u16,							// u16
	token_keyword_u32,							// u32
	token_keyword_u64,							// u64
	token_keyword_f32,							// f32
	token_keyword_f64,							// f64

	token_assignment_equal,						// =
	token_assignment_add_equal,					// +=
	token_assignment_subtract_equal,			// -=
	token_assignment_multiply_equal,			// *=
	token_assignment_divide_equal,				// /=
	token_assignment_modulus_equal,				// %=

	token_arithmetic_add,						// +
	token_arithmetic_subtract,					// -
	token_arithmetic_multiply,					// *
	token_arithmetic_divide,					// /
	token_arithmetic_modulus,					// %

	token_comparison_equal,						// ==
	token_comparison_not_equal,					// !=
	token_comparison_greater_than,				// >
	token_comparison_less_than,					// <
	token_comparison_greater_than_or_equal,		// >=
	token_comparison_less_than_or_equal,		// <=

	token_logical_and,							// &&
	token_logical_or,							// ||
	token_logical_not,							// !

	token_bitwise_and,							// &
	token_bitwise_or,							// |
	token_bitwise_not,							// ~
	token_bitwise_xor,							// ^
	token_bitwise_left_shift,					// <<
	token_bitwise_right_shift,					// >>

	token_punctuation_left_parenth,				// (
	token_punctuation_right_parenth,			// )
	token_punctuation_left_bracket,				// [
	token_punctuation_right_bracket,			// ]
	token_punctuation_left_brace,				// {
	token_punctuation_right_brace,				// }
	token_punctuation_arrow,					// ->
	token_punctuation_semicolon,				// ;
	token_punctuation_colon,					// :
	token_punctuation_comma,					// ,
	token_punctuation_dot,						// .
	token_punctuation_comment,					// #

	tokens_count
} token_e;

typedef struct
{
	string_view_s data;
} identifier_s;

typedef struct
{
	token_e type;
	location_s location;
	string_view_s source;
} token_s;

define_linked_list_type(tokens_list, token_s);

typedef struct
{
	string_view_s file_path;
	string_view_s source;
	location_s location;
} tokenizer_s;

tokenizer_s tokenizer_from_parts(
	const string_view_s file_path);

tokens_list_s tokenizer_process_file(
	tokenizer_s* const tokenizer);

#endif
