
/**
 * @file utils.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-14
 */

#ifndef __primec__include__primec__utils_h__
#define __primec__include__primec__utils_h__

#include <stdint.h>

void* primec_utils_malloc(
	const uint64_t size);

void* primec_utils_realloc(
	void* pointer,
	const uint64_t size);

void primec_utils_free(
	const void* pointer);

void primec_utils_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length);

char* primec_utils_strdup(
	const char* const string);

int32_t primec_utils_strcmp(
	const char* const left,
	const char* const right);

void* primec_utils_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*));

#endif
