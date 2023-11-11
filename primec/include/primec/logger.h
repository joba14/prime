
/**
 * @file logger.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-11
 */

#ifndef __primec__include__primec__logger_h__
#define __primec__include__primec__logger_h__

/**
 * @brief Log tagless level formattable messages.
 */
void primec_logger_log(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log info level formattable messages.
 */
void primec_logger_info(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log warn level formattable messages.
 */
void primec_logger_warn(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log error level formattable messages.
 * 
 */
void primec_logger_error(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
