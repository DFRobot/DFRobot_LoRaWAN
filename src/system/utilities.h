/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Helper functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#ifdef __cplusplus
extern "C"
{
#endif


// #include <Arduino.h>
#include <stdint.h>
/*!
 * Generic definition
 */
// #ifndef SUCCESS
// #define SUCCESS                                     1
// #endif

// #ifndef FAIL
// #define FAIL                                        0
// #endif

/*!
 * \brief Returns the minimum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

/*!
 * \brief Returns the maximum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param  n power value
 * \retval result of raising 2 to the power n
 */
#define POW2(n) (1 << n)

/*!
 * Version
 */
typedef union Version_u
{
    struct Version_s
    {
        uint8_t Revision;
        uint8_t Patch;
        uint8_t Minor;
        uint8_t Major;
    }Fields;
    uint32_t Value;
}Version_t;

/*!
 * \brief Initializes the pseudo random generator initial value
 *
 * \param  seed Pseudo random generator initial value
 */
void srand1(uint32_t seed);

/*!
 * \brief Computes a random number between min and max
 *
 * \param  min range minimum value
 * \param  max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr(int32_t min, int32_t max);

/*!
 * \brief Copies size elements of src array to dst array
 *
 * \remark STM32 Standard memcpy function only works on pointers that are aligned
 *
 * \param  dst  Destination array
 * \param   src  Source array
 * \param   size Number of bytes to be copied
 */
void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Copies size elements of src array to dst array reversing the byte order
 *
 * \param  dst  Destination array
 * \param   src  Source array
 * \param   size Number of bytes to be copied
 */
void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Set size elements of dst array with value
 *
 * \remark STM32 Standard memset function only works on pointers that are aligned
 *
 * \param  dst   Destination array
 * \param   value Default value
 * \param   size  Number of bytes to be copied
 */
void memset1(uint8_t *dst, uint8_t value, uint16_t size);

/*!
 * \brief Converts a nibble to an hexadecimal character
 *
 * \param  a   Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar(uint8_t a);

/** Leaves the minimum of the two 32-bit arguments */
/*lint -emacro(506, MIN) */ /* Suppress "Constant value Boolean */
#define T_MIN(a, b) ((a) < (b) ? (a) : (b))
/** Leaves the maximum of the two 32-bit arguments */
/*lint -emacro(506, T_MAX) */ /* Suppress "Constant value Boolean */
#define T_MAX(a, b) ((a) < (b) ? (b) : (a))
/*!
 * \brief Computes a CCITT 32 bits CRC
 *
 * \param [IN] buffer   Data buffer used to compute the CRC
 * \param [IN] length   Data buffer length
 *
 * \retval crc          The computed buffer of length CRC
 */
uint32_t Crc32( uint8_t *buffer, uint16_t length );

/*!
 * \brief Computes the initial value of the CCITT 32 bits CRC. This function
 *        can be used with functions \ref Crc32Update and \ref Crc32Finalize.
 *
 * \retval crc          Initial crc value.
 */
uint32_t Crc32Init( void );

/*!
 * \brief Updates the value of the crc value.
 *
 * \param [IN] crcInit  Previous or initial crc value.
 * \param [IN] buffer   Data pointer.
 * \param [IN] length   Length of the data.
 *
 * \retval crc          Updated crc value.
 */
uint32_t Crc32Update( uint32_t crcInit, uint8_t *buffer, uint16_t length );

/*!
 * \brief Finalizes the crc value after the calls to \ref Crc32Update.
 *
 * \param [IN] crc      Recent crc value.
 *
 * \retval crc          Updated crc value.
 */
uint32_t Crc32Finalize( uint32_t crc );

/*!
 * Begins critical section
 */
#define CRITICAL_SECTION_BEGIN( ) uint32_t mask; BoardCriticalSectionBegin( &mask )

/*!
 * Ends critical section
 */
#define CRITICAL_SECTION_END( ) BoardCriticalSectionEnd( &mask )

/*
 * ============================================================================
 * Following functions must be implemented inside the specific platform
 * board.c file.
 * ============================================================================
 */
/*!
 * Disable interrupts, begins critical section
 *
 * \param [IN] mask Pointer to a variable where to store the CPU IRQ mask
 */
void BoardCriticalSectionBegin( uint32_t *mask );

/*!
 * Ends critical section
 *
 * \param [IN] mask Pointer to a variable where the CPU IRQ mask was stored
 */
void BoardCriticalSectionEnd( uint32_t *mask );

#ifdef __cplusplus
}
#endif

#endif // __UTILITIES_H__
