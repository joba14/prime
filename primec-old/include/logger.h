
/**
 * @file logger.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __primec__include__logger_h__
#define __primec__include__logger_h__

/**
 * @brief Log info level formattable messages.
 */
void logger_info(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log warn level formattable messages.
 */
void logger_warn(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log error level formattable messages.
 * 
 */
void logger_error(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
