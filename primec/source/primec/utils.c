
/**
 * @file utils.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-14
 */

#include <primec/utils.h>

#include <primec/debug.h>
#include <primec/logger.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

void* primec_utils_malloc(
	const uint64_t size)
{
	primec_debug_assert(size > 0);
	void* const pointer = (void* const)malloc(size);
	if (!pointer && size) { primec_logger_panic("internal failure -- failed to allocate memory"); }
	return pointer;
}

void* primec_utils_realloc(
	void* pointer,
	const uint64_t size)
{
	primec_debug_assert(size > 0);
	pointer = (void*)realloc(pointer, size);
	if (!pointer && size) { primec_logger_panic("internal failure -- failed to reallocate memory"); }
	return pointer;
}

void primec_utils_free(
	const void* pointer)
{
	free((void*)pointer);
	pointer = NULL;
}

void primec_utils_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length)
{
	primec_debug_assert(pointer != NULL);
	(void)memset((void*)pointer, value, length);
}

void primec_utils_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length)
{
	primec_debug_assert(destination != NULL);
	primec_debug_assert(source != NULL);
	primec_debug_assert(length > 0);
	(void)memcpy((void*)destination, (const void*)source, length);
}

char* primec_utils_strdup(
	const char* const string)
{
	primec_debug_assert(string != NULL);
	char* const result = strdup(string);
	if (!result) { primec_logger_panic("internal failure -- failed to duplicate (and allocate) string"); }
	return result;
}

char* primec_utils_strndup(
	const char* const string,
	const uint64_t length)
{
	primec_debug_assert(string != NULL);
	primec_debug_assert(length > 0);
	char* const result = strndup(string, length);
	if (!result) { primec_logger_panic("internal failure -- failed to duplicate (and allocate) string"); }
	return result;
}

int32_t primec_utils_strcmp(
	const char* const left,
	const char* const right)
{
	primec_debug_assert(left != NULL);
	primec_debug_assert(right != NULL);
	return strcmp((const char*)left, (const char*)right);
}

void* primec_utils_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*))
{
	return bsearch(key, base, members_count, member_size, compare);
}
