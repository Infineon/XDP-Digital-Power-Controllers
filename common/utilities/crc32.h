/* ============================================================================
** Original author
** Stephan Brumme
** Rudolf-Breitscheid-Str. 226
** 14482 Potsdam
** Germany
** create@stephan-brumme.com
** ============================================================================
**
** ============================================================================
** License
** All source code published on http://create.stephan-brumme.com and its
** sub-pages is licensed similar to the zlib license:
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the author be held liable for any damages arising from the
** use of this software.
** Permission is granted to anyone to use this software for any purpose,
** including commercial applications, and to alter it and redistribute it
** freely, subject to the following restrictions:
**
** 1. The origin of this software must not be misrepresented; you must not
**    claim that you wrote the original software.
** 2. If you use this software in a product, an acknowledgment in the product
**    documentation would be appreciated but is not required.
** 3. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the original software.
** ============================================================================
*/
/**
 * @file    crc32.h
 * @brief   API for using the CRC32 algorithm.
 */

#ifndef CRC32_H
#define CRC32_H

/* Data types */
#include <stdint.h>     // uint*_t

#define HWCRC32_POLYNOMIAL (0x4C11DB7UL)

/**
 * Fast CRC32: Half-Byte Algorithm.
 * For certain embedded CPUs the bitwise algorithm might be too slow but
 * there is not enough memory available for the look-up tables of the faster
 * algorithms either (between 1k and 8k). The half-byte/nibble algorithm is a
 * sweet compromise:
 * its look-up table contains only 16 entries for a total of 64 bytes and runs
 * about three times faster than the best bitwise algorithm.
 *
 * @note Code from [here](http://create.stephan-brumme.com/crc32/#half-byte)
 *
 * @param data      Data of arbitrary type from which the checksum is calculated
 * @param length    Number of @p data bytes (up to 64KB)
 * @param previous_crc32  Checksum of ongoing calculation for generating a
 *                        checksum of separated data blocks.
 *                        Set to 0 for the first or a single data block.
 * @return 32-bit checksum of @p data.
 */
uint32_t crc32_halfbyte(const void* data, uint16_t length,
                        uint32_t previous_crc32);

/**
 * HW CRC32: Byte Algorithm.
 * Configuration corresponds to a standard predefined CRC32
 * algorithm
 * Input reflected = yes
 * Result reflected = yes
 * Polynomial = 0x04C11DB7
 * Initial value = 0xFFFFFFFF
 * Final xor value = 0xFFFFFFFF
 * @note see http://www.sunshine2k.de/coding/javascript/crc/crc_js.html *
 *
 * @param data      Point to data of arbitrary type from which the checksum is calculated
 * @param length    Number of @p data bytes (up to 64KB)
 * @param previous_crc32  Currently not functional, set to 0!
 *                        Checksum of ongoing calculation for generating a
 *                        checksum of separated data blocks.
 *                        Set to 0 for the first or a single data block.
 *
 * @return 32-bit checksum of @p data.
 */
uint32_t hw_crc32(const void* data, uint16_t length,
                        uint32_t previous_crc32);

#endif /* CRC32_H */
