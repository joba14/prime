
/**
 * @file tokenizer.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#include <tokenizer.h>

#include <debug.h>
#include <logger.h>

#include <stddef.h>
#include <stdio.h>

implement_linked_list_type(tokens_list, token_s);

#define cstr2sv(_cstring) (string_view_s) { .data = _cstring, .length = ((sizeof(_cstring) / sizeof(char)) - 1) }

static const string_view_s g_stringified_tokens[tokens_count] =
{
	[token_invalid]									= cstr2sv("invalid"),
	[token_identifier]								= cstr2sv("identifier"),
	[token_literal_c8]								= cstr2sv("literal-c8"),
	[token_literal_i8]								= cstr2sv("literal-i8"),
	[token_literal_i16]								= cstr2sv("literal-i16"),
	[token_literal_i32]								= cstr2sv("literal-i32"),
	[token_literal_i64]								= cstr2sv("literal-i64"),
	[token_literal_u8]								= cstr2sv("literal-u8"),
	[token_literal_u16]								= cstr2sv("literal-u16"),
	[token_literal_u32]								= cstr2sv("literal-u32"),
	[token_literal_u64]								= cstr2sv("literal-u64"),
	[token_literal_f32]								= cstr2sv("literal-f32"),
	[token_literal_f64]								= cstr2sv("literal-f64"),
	[token_literal_str]								= cstr2sv("literal-str"),
	[token_keyword_let]								= cstr2sv("keyword-let"),							// let
	[token_keyword_ref]								= cstr2sv("keyword-ref"),							// ref
	[token_keyword_mut]								= cstr2sv("keyword-mut"),							// mut
	[token_keyword_unsafe]							= cstr2sv("keyword-unsafe"),						// unsafe
	[token_keyword_if]								= cstr2sv("keyword-if"),							// if
	[token_keyword_elif]							= cstr2sv("keyword-elif"),							// elif
	[token_keyword_else]							= cstr2sv("keyword-else"),							// else
	[token_keyword_while]							= cstr2sv("keyword-while"),						// while
	[token_keyword_break]							= cstr2sv("keyword-break"),						// break
	[token_keyword_continue]						= cstr2sv("keyword-continue"),						// continue
	[token_keyword_return]							= cstr2sv("keyword-return"),						// return
	[token_keyword_func]							= cstr2sv("keyword-func"),							// func
	[token_keyword_inl]								= cstr2sv("keyword-inl"),							// inl
	[token_keyword_ext]								= cstr2sv("keyword-ext"),							// ext
	[token_keyword_struct]							= cstr2sv("keyword-struct"),						// struct
	[token_keyword_alias]							= cstr2sv("keyword-alias"),						// alias
	[token_keyword_c8]								= cstr2sv("keyword-c8"),							// c8
	[token_keyword_i8]								= cstr2sv("keyword-i8"),							// i8
	[token_keyword_i16]								= cstr2sv("keyword-i16"),							// i16
	[token_keyword_i32]								= cstr2sv("keyword-i32"),							// i32
	[token_keyword_i64]								= cstr2sv("keyword-i64"),							// i64
	[token_keyword_u8]								= cstr2sv("keyword-u8"),							// u8
	[token_keyword_u16]								= cstr2sv("keyword-u16"),							// u16
	[token_keyword_u32]								= cstr2sv("keyword-u32"),							// u32
	[token_keyword_u64]								= cstr2sv("keyword-u64"),							// u64
	[token_keyword_f32]								= cstr2sv("keyword-f32"),							// f32
	[token_keyword_f64]								= cstr2sv("keyword-f64"),							// f64
	[token_assignment_equal]						= cstr2sv(""),						// =
	[token_assignment_add_equal]					= cstr2sv(""),					// +=
	[token_assignment_subtract_equal]				= cstr2sv(""),			// -=
	[token_assignment_multiply_equal]				= cstr2sv(""),			// *=
	[token_assignment_divide_equal]					= cstr2sv(""),				// /=
	[token_assignment_modulus_equal]				= cstr2sv(""),				// %=
	[token_arithmetic_add]							= cstr2sv(""),						// +
	[token_arithmetic_subtract]						= cstr2sv(""),					// -
	[token_arithmetic_multiply]						= cstr2sv(""),					// *
	[token_arithmetic_divide]						= cstr2sv(""),					// /
	[token_arithmetic_modulus]						= cstr2sv(""),					// %
	[token_comparison_equal]						= cstr2sv(""),						// ==
	[token_comparison_not_equal]					= cstr2sv(""),					// !=
	[token_comparison_greater_than]					= cstr2sv(""),				// >
	[token_comparison_less_than]					= cstr2sv(""),					// <
	[token_comparison_greater_than_or_equal]		= cstr2sv(""),		// >=
	[token_comparison_less_than_or_equal]			= cstr2sv(""),		// <=
	[token_logical_and]								= cstr2sv(""),							// &&
	[token_logical_or]								= cstr2sv(""),							// ||
	[token_logical_not]								= cstr2sv(""),							// !
	[token_bitwise_and]								= cstr2sv(""),							// &
	[token_bitwise_or]								= cstr2sv(""),							// |
	[token_bitwise_not]								= cstr2sv(""),							// ~
	[token_bitwise_xor]								= cstr2sv(""),							// ^
	[token_bitwise_left_shift]						= cstr2sv(""),					// <<
	[token_bitwise_right_shift]						= cstr2sv(""),					// >>
	[token_punctuation_left_parenth]				= cstr2sv(""),				// (
	[token_punctuation_right_parenth]				= cstr2sv(""),			// )
	[token_punctuation_left_bracket]				= cstr2sv(""),				// [
	[token_punctuation_right_bracket]				= cstr2sv(""),			// ]
	[token_punctuation_left_brace]					= cstr2sv(""),				// {
	[token_punctuation_right_brace]					= cstr2sv(""),				// }
	[token_punctuation_arrow]						= cstr2sv(""),					// ->
	[token_punctuation_semicolon]					= cstr2sv(""),				// ;
	[token_punctuation_colon]						= cstr2sv(""),					// :
	[token_punctuation_comma]						= cstr2sv(""),					// ,
	[token_punctuation_dot]							= cstr2sv(""),						// .
	[token_comment_single_line]						= cstr2sv(""),					// //
	[token_comment_multi_line_left]					= cstr2sv(""),				// /*
	[token_comment_multi_line_right]				= cstr2sv(""),				// */
};

tokenizer_s tokenizer_from_parts(
	const string_view_s file_path)
{
	debug_assert_string_view(file_path);

	return (tokenizer_s)
	{
		.file_path = file_path,
		.source = {0},
		.location = (location_s)
		{
			.file = file_path,
			.line = 0,
			.column = 0
		}
	};
}

tokens_list_s tokenizer_process_file(
	tokenizer_s* const tokenizer)
{
	debug_assert(tokenizer != NULL);

	tokens_list_s tokens = tokens_list_from_parts();

	FILE* const file = fopen(tokenizer->file_path.data, "rt");

	if (NULL == file)
	{
		logger_error("failed to open file `" sv_fmt "`!", sv_arg(tokenizer->file_path));
		return tokens;
	}

	// TODO: implement!

	

	return tokens;
}
