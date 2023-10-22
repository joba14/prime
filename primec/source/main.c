
/**
 * @file main.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#include <main.h>

#include <debug.h>
#include <logger.h>

#include <string_view.h>
#include <linked_list.h>
#include <heap_buffer.h>

#include <options.h>
#include <tokenizer.h>

#include <stdint.h>
#include <stddef.h>

signed int main(
	const signed int argc,
	const char** const argv)
{
	bool with_outputs = false;
	options_s options = options_from_parts();
	options_parse(&options, (uint64_t)argc, argv, &with_outputs);

	for (options_node_s* iterator = options.head; iterator != NULL; iterator = iterator->next)
	{
		const string_view_s input_file_path = iterator->data;

		tokenizer_s tokenizer = tokenizer_from_parts(input_file_path);
		tokens_list_s tokens = tokenizer_process_file(&tokenizer);
		(void)tokens;
		// TODO: tokens!

		string_view_s output_file_path = input_file_path;

		if (with_outputs)
		{
			iterator = iterator->next;
			output_file_path = iterator->data;
		}

		logger_info(sv_fmt " -> " sv_fmt,
			sv_arg(input_file_path), sv_arg(output_file_path)
		);
	}

	return 0;
}
