
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

#ifndef __PRIMEC__INCLUDE__LOGGER_H__
#define __PRIMEC__INCLUDE__LOGGER_H__

void logger_info(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

void logger_warn(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

void logger_error(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
