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
typedef uint8_t bi_word;
#elif defined(BI_WORD32)
typedef uint16_t bi_word;
#else
typedef uint32_t bi_word;
#endif
	
// Defining base type
typedef enum {
    BINARY = 2,
    DECIMAL = 10,
    HEXADECIMAL = 16
} base;

// Defining sign type
typedef enum {
    POSITIVE = 0,
    NEGATIVE = 1
} sign;

// Defining error code type
typedef enum {
    NO_ERROR = 0, 
    BASE_IS_INCORRECT = 1,
    FILE_IS_NOT_OPENED = 2,
    ERROR_CODE3 = 3
} error;

// Define the big integer structure
typedef struct {
    sign sign;     // Sign of the big integer. Signed: 1, Unsigned: 0
    bi_word dmax;  // Maximum number of words in the array
    bi_word* a;    // Array of words representing the big integer
} bi;

// Initialize a new big integer
void bi_new(bi** x, bi_word dmax);

// Free the memory used by a big integer
void bi_delete(bi** x);

// Refine the big integer (provide a comment explaining what refining means)
void bi_refine(bi* x);

// Display the big integer in binary format
void bi_show_bin(const bi* x);

// Display the big integer in hexadecimal format
void bi_show_hex(const bi* x);

// Set a big integer from an array of words
error bi_set_by_arr(bi** x, const bi_word* a, sign sign, bi_word dmax);

// Set a big integer from a string representation
error bi_set_by_str(bi** x, const char* str, sign sign, base base);

// Set a big integer to zero
void bi_set_zero(bi** x);

// Set a big integer to one
void bi_set_one(bi** x);

// Copy a big integer to another
void bi_cpy(bi** y, const bi* x);

// Compare two big integers and return the result
bi_word bi_cmp(const bi* x, const bi* y);

// Generate a random big integer
void bi_gen_rend(bi** x, sign sign, bi_word dmax);

// Check if a big integer is zero
bool bi_is_zero(const bi* x);

// Check if a big integer is one
bool bi_is_one(const bi* x);

#ifdef __cplusplus
}
#endif

#endif
