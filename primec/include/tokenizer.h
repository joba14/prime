
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

typedef struct
{
	void * dummy_;
} token_s;

define_linked_list_type(tokens_list, token_s);

typedef struct
{
	string_view_s file_path;
} tokenizer_s;

tokenizer_s tokenizer_from_parts(
	const string_view_s file_path);

tokens_list_s tokenizer_process_file(
	tokenizer_s* const tokenizer);

#endif
