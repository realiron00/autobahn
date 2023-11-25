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
#include <stdarg.h>

#define BI_WORD8

// Define the word size based on preprocessor macros
#if defined(BI_WORD8)
    typedef uint8_t Word;  ///< Unsigned 16-bit word for big integers.
    #define PRINT_WORD_FORMAT "%x"
    #define PRINT_WORD_FIX_FORMAT "%02x"
#elif defined(BI_WORD64)
    typedef uint64_t Word; ///< Unsigned 32-bit word for big integers.
    #define PRINT_WORD_FORMAT "%llx"
    #define PRINT_WORD_FIX_FORMAT "%16llx"
#else
    typedef uint32_t Word;  ///< Unsigned 64-bit word for big integers.
    #define PRINT_WORD_FORMAT "%x"
    #define PRINT_WORD_FIX_FORMAT "%08x"
#endif

#define SIZE_OF_WORD sizeof(Word)
#define SIZE_OF_BIGINT sizeof(Bigint)
#define BITLEN_OF_WORD (sizeof(Word) * 8)
#define MASK1BIT 0x01
#define MASK8BIT 0xFF
#define SAME 0
#define LEFT_IS_BIG 1
#define LEFT_IS_SMALL -1
#define FALSE 0
#define TRUE 1

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
} Sign;

/**
 * @struct bigint
 * @brief Custom Big Integer (bigint) structure.
 *
 * @details This structure represents a custom big integer type with sign information,
 * maximum number of digits (Word), and an array to store the digits.
 */
typedef struct {
    Sign sign;       ///< Sign of the big integer (positive or negative).
    Word digit_num;  ///< Maximum number of digits in the big integer.
    Word* digits;    ///< Array to store the digits of the big integer.
} Bigint;

void bigint_new(Bigint** bigint, Word new_digit_num);
void bigint_delete(Bigint** bigint);
void bigint_refine(Bigint* bigint);
void bigint_set_by_hex_string(Bigint** bigint, const char* string, Sign sign);
void bigint_copy(Bigint** bigint_dest, const Bigint* bigint_src);
void bigint_generate_random_number(Bigint** bigint, Sign sign, Word digit_num);
void bigint_show_hex(const Bigint* bigint);
char bigint_compare(const Bigint* operand_x, const Bigint* operand_y);
char bigint_compare_abs(const Bigint* operand_x, const Bigint* operand_y);
void bigint_expand(Bigint** result, const Bigint* bigint, Word wordlen);
void bigint_compress(Bigint** result, const Bigint* bigint, Word wordlen);
void bigint_copy_part(Bigint** result, const Bigint* bigint, Word offset_start, Word offset_end);
Word bigint_get_bit_length(const Bigint* bigint);
Word bigint_get_bit(const Bigint* bigint, Word bit_idx);
void bigint_expand_one_bit(Bigint** result, const Bigint* bigint);
void bigint_compress_one_bit(Bigint** result, const Bigint* bigint);
void bigint_set_zero(Bigint** bigint);
void bigint_set_one(Bigint** bigint);
char bigint_is_zero(const Bigint* bigint);
char bigint_is_one(const Bigint* bigint);

#endif
