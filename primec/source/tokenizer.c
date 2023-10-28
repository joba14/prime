
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

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

implement_linked_list_type(tokens_list, token_s);

#define cstr2sv(_cstring) { .data = _cstring, .length = ((sizeof(_cstring) / sizeof(char)) - 1) }

static const string_view_s g_stringified_tokens[tokens_count] =
{
	[token_comment_single_line] = 			cstr2sv("//"),
	[token_comment_multi_line_left] = 		cstr2sv("/*"),
	[token_comment_multi_line_right] = 		cstr2sv("*/"),
};

static bool read_whole_file(
	tokenizer_s* const tokenizer);

static bool try_parse_token(
	tokenizer_s* const tokenizer,
	token_s* const token);

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

	if (!read_whole_file(tokenizer))
	{
		return tokens;
	}

	token_s token;

	while (try_parse_token(tokenizer, &token))
	{
		tokens_list_push_tail(&tokens, token);
	}

	return tokens;
}

static bool read_whole_file(
	tokenizer_s* const tokenizer)
{
	debug_assert(tokenizer != NULL);
	debug_assert_string_view(tokenizer->file_path);

	FILE* const file = fopen(tokenizer->file_path.data, "rt");

	if (NULL == file)
	{
		logger_error("failed to open file `" sv_fmt "`!", sv_arg(tokenizer->file_path));
		fclose(file);
		return false;
	}

	fseek(file, 0, SEEK_END);
	const uint64_t length = (uint64_t)ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = (char*)malloc(length);
	debug_assert(buffer != NULL);
	fread(buffer, 1, length, file);

	tokenizer->source = string_view_from_parts(buffer, length);
	fclose(file);
	return true;
}

static bool try_parse_token(
	tokenizer_s* const tokenizer,
	token_s* const token)
{
	debug_assert(tokenizer != NULL);
	debug_assert(token != NULL);

	tokenizer->source = string_view_trim_left(tokenizer->source);

	for (uint64_t index = 0; index < tokens_count; ++index)
	{
		const string_view_s stringified_token = g_stringified_tokens[index];

		if (string_view_equal_range(tokenizer->source, stringified_token, stringified_token.length))
		{
			switch (index)
			{
				case token_comment_single_line:
				{
					
				} break;

				case token_comment_multi_line_left:
				{
				} break;

				case token_comment_multi_line_right:
				{
				} break;

				default:
				{
				} break;
			}
		}
	}

	system("clear");
	fprintf(stdout, sv_fmt, sv_arg(tokenizer->source));
	(void)getchar();

	return true;
}
