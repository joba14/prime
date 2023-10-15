
/**
 * @file prime_logger.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-01
 */

#ifndef __PRIME_COMMON__INCLUDE__PRIME_COMMON__PRIME_LOGGER_H__
#define __PRIME_COMMON__INCLUDE__PRIME_COMMON__PRIME_LOGGER_H__

void prime_logger_info(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

void prime_logger_warn(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

void prime_logger_error(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
