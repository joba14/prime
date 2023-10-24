
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
