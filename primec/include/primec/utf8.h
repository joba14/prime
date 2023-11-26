
/**
 * @file utf8.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-14
 */

#ifndef __primec__include__primec__utf8_h__
#define __primec__include__primec__utf8_h__

#include <stdint.h>
#include <limits.h>
#include <stdio.h>

typedef uint32_t utf8char_t;
#define primec_utf8_max_size sizeof(utf8char_t)
#define primec_utf8_invalid UINT32_MAX

utf8char_t primec_utf8_decode(
	const char** const string);

uint8_t primec_utf8_encode(
	char* const string,
	utf8char_t utf8char);

utf8char_t primec_utf8_get(
	FILE* const file);

#endif
