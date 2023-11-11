
/**
 * @file version.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-11
 */

#ifndef __primec__include__primec__version_h__
#define __primec__include__primec__version_h__

#define primec_version_major 1
#define primec_version_minor 0
#define primec_version_patch 0

#include <stdint.h>
#define primec_version_fmt "v%lu.%lu.%lu"
#define primec_version_arg (uint64_t)primec_version_major, (uint64_t)primec_version_minor, (uint64_t)primec_version_patch

#endif
