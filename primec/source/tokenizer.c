
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

implement_linked_list_type(tokens_list, token_s);

tokenizer_s tokenizer_from_parts(
	const string_view_s file_path)
{
	return (tokenizer_s)
	{
		.file_path = file_path
	};
}

tokens_list_s tokenizer_process_file(
	tokenizer_s* const tokenizer)
{
	(void)tokenizer;
	tokens_list_s tokens = tokens_list_from_parts();
	// TODO: implement!
	return tokens;
}
