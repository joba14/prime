
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

/**
 * @brief Decode utf-8 char from provided string.
 * 
 * @note In case function fails to decode utf-8 char from the provided string,
 * it returns "primec_utf8_invalid".
 */
utf8char_t primec_utf8_decode(
	const char** const string);

/**
 * @brief Encode utf-8 char into a string.
 * 
 * @note The max size of the encoded buffer can be 4 bytes. The actual size the
 * function required to encode the utf8 char is returned from the function.
 */
uint8_t primec_utf8_encode(
	char* const string,
	utf8char_t utf8char);

/**
 * @brief Get utf-8 char from provided file stream.
 * 
 * @note In case function fails to decode utf-8 char from the provided string,
 * it returns "primec_utf8_invalid".
 */
utf8char_t primec_utf8_get(
	FILE* const file);

#endif
