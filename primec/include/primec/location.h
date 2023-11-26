
/**
 * @file location.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-20
 */

#ifndef __primec__include__primec__location_h__
#define __primec__include__primec__location_h__

#include <stdint.h>

typedef struct
{
	const char* file;
	uint64_t line;
	uint64_t column;
} primec_location_s;

/**
 * @brief Location formatting macro for printf-like functions.
 */
#define primec_location_fmt "%s:%lu:%lu"

/**
 * @brief Location formatting argument macro for printf-like functions.
 */
#define primec_location_arg(_location) \
	(_location).file, (_location).line, (_location).column

#endif
