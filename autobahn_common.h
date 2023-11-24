/**
 * @file autobahn_common.h
 * @author DH KIM (eastchord0729@gmail.com),
 *         KH KIM (    ),
 *         JC PARK (    ),
 *         SH HAN (    )
 * @brief Common Definitions and Functions for Autobahn Project.
 * 
 * This header file contains common definitions and functions that are shared
 * across the Autobahn project. It serves as a central repository for common
 * elements such as data structures, constants, and utility functions.
 * 
 * @version 0.1
 * @date 2023-10-06
 * 
 * @note
 * This header file is subject to change and extension as the Autobahn project
 * evolves and additional functionality is added.
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef AUTOBAHN_COMMON_H
#define AUTOBAHN_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define the word size based on preprocessor macros
#if defined(BI_WORD16)
typedef int8_t bi_word;    ///< Signed 16-bit word for big integers.
typedef uint8_t bi_uword;  ///< Unsigned 16-bit word for big integers.
#elif defined(BI_WORD32)
typedef int16_t bi_word;   ///< Signed 32-bit word for big integers.
typedef uint16_t bi_uword; ///< Unsigned 32-bit word for big integers.
#else
typedef int32_t bi_word;    ///< Signed 64-bit word for big integers.
typedef uint32_t bi_uword;  ///< Unsigned 64-bit word for big integers.
#endif

#define BITLEN_OF_WORD (sizeof(bi_word) * 8)

/**
 * @enum base
 * @brief Enumeration representing different numerical bases.
 *
 * @details This enumeration defines constants for common numerical bases such as binary,
 * decimal, and hexadecimal.
 */
typedef enum {
    BINARY = 2,        ///< Binary base (base 2).
    DECIMAL = 10,      ///< Decimal base (base 10).
    HEXADECIMAL = 16   ///< Hexadecimal base (base 16).
} base;

/**
 * @enum sign
 * @brief Enumeration representing the sign of a value.
 *
 * @details This enumeration defines constants for representing the sign of a value,
 * such as positive or negative.
 */
typedef enum {
    POSITIVE = 0,  ///< Represents a positive value.
    NEGATIVE = 1   ///< Represents a negative value.
} sign;

/**
 * @struct bi
 * @brief Custom Big Integer (bi) structure.
 *
 * @details This structure represents a custom big integer type with sign information,
 * maximum number of digits (bi_uword), and an array to store the digits.
 */
typedef struct {
    sign sign;      ///< Sign of the big integer (positive or negative).
    bi_uword dmax;  ///< Maximum number of digits in the big integer.
    bi_uword* a;    ///< Array to store the digits of the big integer.
} bi;

/**
 * @brief Initialize a new big integer
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 * @param dmax Maximum number of digits for the big integer.
 */
void bi_new(bi** x, bi_uword dmax);

/**
 * @brief Free the memory used by a big integer
 * 
 * @param x Pointer to a pointer to a bi structure.
 */
void bi_delete(bi** x);

/**
 * @brief Refine the big integer (provide a comment explaining what refining means)
 * 
 * @param x Pointer to a bi structure.
 */
void bi_refine(bi* x);

/**
 * @brief Display the big integer in binary format
 * 
 * @param x Pointer to a bi structure.
 */
void bi_show_bin(const bi* x);

//TODO Comming Soon...
/**
 * @brief Display the big integer in decimal format
 * 
 * @param x Pointer to a bi structure.
 */
void bi_show_dec(const bi* x);

/**
 * @brief Display the big integer in hexadecimal format
 * 
 * @param x Pointer to a bi structure.
 */
void bi_show_hex(const bi* x);

/**
 * @brief Set a big integer from an array of words
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 * @param arr Array of bi_uword representing the big integer.
 * @param sign Sign of the big integer (positive or negative).
 * @param dmax Maximum number of digits for the big integer.
 */
void bi_set_by_arr(bi** x, const bi_uword* arr, sign sign, bi_uword dmax);

/**
 * @brief Set a big integer from a string representation
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 * @param str String representation of the big integer.
 * @param sign Sign of the big integer (positive or negative).
 * @param base Numerical base (binary, decimal, or hexadecimal).
 */
void bi_set_by_str(bi** x, const char* str, sign sign, base base);

/**
 * @brief Set a big integer to zero
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 */
void bi_set_zero(bi** x);

/**
 * @brief Set a big integer to one
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 */
void bi_set_one(bi** x);

/**
 * @brief Copy a big integer to another
 * 
 * @param y Pointer to a pointer to a bi structure (output parameter).
 * @param x Pointer to a bi structure to be copied.
 */
void bi_cpy(bi** y, const bi* x);

/**
 * @brief Compare two big integers and return the result
 * 
 * @param x Pointer to a bi structure.
 * @param y Pointer to a bi structure to be compared with x.
 * @return bi_word Result of the comparison (negative, zero, or positive).
 */
bi_word bi_cmp(const bi* x, const bi* y);

/**
 * @brief Generate a random big integer
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter).
 * @param sign Sign of the generated big integer (positive or negative).
 * @param dmax Maximum number of digits for the generated big integer.
 */
void bi_gen_rand(bi** x, sign sign, bi_uword dmax);

/**
 * @brief Check if a big integer is zero
 * 
 * @param x Pointer to a bi structure.
 * @return true if the big integer is zero, false otherwise.
 */
bool bi_is_zero(const bi* x);

/**
 * @brief Check if a big integer is one
 * 
 * @param x Pointer to a bi structure.
 * @return true if the big integer is one, false otherwise.
 */
bool bi_is_one(const bi* x);

#ifdef __cplusplus
}
#endif

#endif
