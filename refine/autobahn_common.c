#include "autobahn_common.h"

/**
 * @brief Allocates memory for a large integer.
 * 
 * This function allocates memory for a large integer of size [new_digit_num]
 * and stores the address in the [bigint] variable. If you want to use
 * the integer 0xFFFFFFFFFFFF in a 32-bit environment, set new_digit_num to 2.
 * 
 * @param bigint [output] Memory address of the large integer.
 * @param new_digit_num [input] Number of digits.
 */
void bigint_new(Bigint** bigint, Word new_digit_num)
{
    /* Free allocated memory */
    if (*bigint != NULL) 
        bigint_delete(bigint);

    /* Size is at least 1 */
    if (new_digit_num == 0) new_digit_num = 1;

    /* Allocate Bigint */
    *bigint = (Bigint*)malloc(SIZE_OF_BIGINT);
    (*bigint)->sign = POSITIVE;
    (*bigint)->digit_num = new_digit_num;
    (*bigint)->digits = (Word*)calloc(new_digit_num, SIZE_OF_WORD);
}

/**
 * @brief Releases memory allocated for a large integer.
 * 
 * This function frees the memory allocated for a large integer if the 
 * memory address is stored in [bigint]. If memory for a large integer
 * is allocated, it is essential to release that memory at the end.
 *
 * @param bigint [input] Memory address of the large integer.
 */
void bigint_delete(Bigint** bigint)
{
    /* Invalid pointer */
    if (*bigint == NULL) 
        return;

    /* Free memory */
    free((*bigint)->digits);
    free(*bigint);
}

/**
 * @brief Free unused memory.
 * 
 * Frees unused memory in the allocated large integer memory of [bigint] 
 * if the integer stored in the memory is not large enough to utilize the upper addresses.
 * Additionally, adjusts the memory size and structure of the large integer, 
 * taking into account scenarios such as 0xFFFFFFFFFFFF in a 32-bit environment with digit_num as 3.
 * 
 * @param bigint [input/output] Memory address of the large integer.
 */
void bigint_refine(Bigint* bigint)
{
    /* Invalid pointer */
    if (bigint == NULL) return;

    /* New number of digits */
    Word new_digit_num = bigint->digit_num;

    /* Calculate the new number of digits */
    while (new_digit_num > 1) {
        if (bigint->digits[new_digit_num - 1] != 0) break; // Stop if an integer is found from the top
        new_digit_num--;
    }

    /* Check if refinement is needed */
    if (bigint->digit_num == new_digit_num) return;

    /* Refine memory */
    bigint->digit_num = new_digit_num;
    bigint->digits = (Word*)realloc(bigint->digits, SIZE_OF_WORD * new_digit_num);

    /* Zero is always positive */
    if (bigint_is_zero(bigint)) 
        bigint->sign = POSITIVE;
}

/**
 * @brief Displays the hexadecimal representation of a large integer.
 * 
 * This function prints the hexadecimal representation of the large integer 
 * specified by [bigint] to the standard output.
 * 
 * @param bigint [input] The large integer.
 */
void bigint_show_hex(const Bigint* bigint) 
{
    /* Negative format */
    if (bigint->sign) printf("-");

    /* Hexadecimal format */
    printf("0x");

    /* Print the first digit */
    printf(PRINT_WORD_FORMAT, bigint->digits[bigint->digit_num - 1]);

    /* Print the digits */
    for (int idx = bigint->digit_num - 2; idx >= 0; idx--)
        printf(PRINT_WORD_FIX_FORMAT, bigint->digits[idx]);

    /* line break */
    printf("\n");
}

/**
 * @brief Sets the value of a large integer from a hexadecimal string.
 * 
 * This function allocates memory for a new large integer specified by [bigint],
 * initializes it with the hexadecimal values provided in the string [string], and
 * sets the sign according to [sign].
 * 
 * The input string [string] represents a hexadecimal number. It is converted to a
 * large integer [bigint] by processing the string in chunks of 4 bits (nibbles).
 * 
 * @param bigint [output] The new large integer.
 * @param string [input] Hexadecimal string.
 * @param sign [input] Sign of the large integer.
 */
void bigint_set_by_hex_string(Bigint** bigint, const char* string, Sign sign)
{
    /* Get length of string and new digit number */
    Word length_string = (Word)strlen(string);
    Word new_digit_num = (length_string / (SIZE_OF_WORD * 2)) + (length_string % (SIZE_OF_WORD * 2) != 0);

    /* Allocate Bigint */
    bigint_new(bigint, new_digit_num);
    (*bigint)->sign = sign;

    /* String to Bigint */
    for (int idx = 0; idx < new_digit_num; idx++) 
    {
        /* Char to int */
        for (int idx_4bit = 0; idx_4bit < BITLEN_OF_WORD; idx_4bit += 4)
        {
            char char_1byte = string[--length_string];
            Word int_4bit;

            /* One byte char to 4-bits int */
            if ('0' <= char_1byte && char_1byte <= '9') int_4bit = char_1byte - '0';
            if ('A' <= char_1byte && char_1byte <= 'F') int_4bit = char_1byte - 'A' + 10;
            if ('a' <= char_1byte && char_1byte <= 'f') int_4bit = char_1byte - 'a' + 10;

            /* Fill in the digit */
            (*bigint)->digits[idx] += int_4bit << idx_4bit;

            /* String is gone */
            if (length_string == 0) break;
        }
    }

    /* Free unuse memory */
    bigint_refine(*bigint);
}

/**
 * @brief Copies the content of one large integer to another.
 * 
 * Stores it in the memory allocated for [bigint_dest] using [bigint_src].
 * 
 * @param bigint_dest [output] The destination variable for the copied large integer.
 * @param bigint_src [input] The source large integer to be copied.
 */
void bigint_copy(Bigint** bigint_dest, const Bigint* bigint_src)
{ 
    /* Allocate new Bigint */
    Word new_digit_num = bigint_src->digit_num;
    bigint_new(bigint_dest, new_digit_num);

    /* Copy the digits */
    for (int idx = 0; idx < new_digit_num; idx++)
        (*bigint_dest)->digits[idx] = bigint_src->digits[idx];

    /* Copy the sign */
    (*bigint_dest)->sign = bigint_src->sign;
}

/**
 * @brief Copies a part of a large integer into a new large integer.
 * 
 * We will use this function in Karatsuba multiplication to divide the operands.
 * Note that the result is always POSITIVE.
 * 
 * @param result [output] The new large integer.
 * @param bigint [input] The source large integer.
 * @param offset_start [input] Starting offset of digits.
 * @param offset_end [input] Ending offset of digits.
 */
void bigint_copy_part(Bigint** result, const Bigint* bigint, Word offset_start, Word offset_end) 
{
    /* Invalid offset */
	if (bigint->digit_num < offset_end || bigint->digit_num < offset_start || offset_end < offset_start) {
        printf("invalid offset\n");
        bigint_copy(result, bigint);
        return;
    }

    /* Allocate Bigint */
    Word new_digit_num = offset_end - offset_start;
    bigint_new(result, new_digit_num);

    /* copy the digits */
    for(int i = 0; i < new_digit_num; i++)
        (*result)->digits[i] = bigint->digits[offset_start + i];

    bigint_refine(*result);
}

char bigint_compare_abs(const Bigint* operand_x, const Bigint* operand_y)
{
    //Return 1 if operand_x's dmax is bigger than operand_y's dmax
    if (operand_x->digit_num > operand_y->digit_num) return 1;
    if (operand_x->digit_num < operand_y->digit_num) return -1;

    //if operand_x->digit_num == y_dmax, compare each digit
    for (int idx = operand_x->digit_num - 1; idx >= 0; idx--) 
    {
        //Return 1 if the value of the digit in operand_x is bigger than the value of the digit in operand_y
        if (operand_x->digits[idx] > operand_y->digits[idx]) return 1;
        if (operand_x->digits[idx] < operand_y->digits[idx]) return -1;
    }

    //if all the digits are the same, return 0
    return 0;
}

/**
 * @brief Compares two large integers.
 * 
 * This function compares the values of two large integers, [operand_x] and [operand_y].
 * It considers the signs and number of digits first. 
 * and Finally considers the individual word integers to determine 
 * the relationship between the two large integers.
 * 
 * @param operand_x [input] First large integer for comparison.
 * @param operand_y [input] Second large integer for comparison.
 * @return char A value indicating the relationship between the two large integers:
 *              -1 if operand_x < operand_y,
 *               0 if operand_x equals operand_y,
 *               1 if operand_x > operand_y.
 */
char bigint_compare(const Bigint* operand_x, const Bigint* operand_y)
{
    //Return 1 if operand_x is positive and operand_y is negative
    if (operand_x->sign == POSITIVE && operand_y->sign == NEGATIVE) return 1;
    if (operand_x->sign == NEGATIVE && operand_y->sign == POSITIVE) return -1;

    //Compare the absolute values of operand_x and operand_y
    char who_is_big = bigint_compare_abs(operand_x, operand_y);

    //If operand_x and operand_y are both positive, return the value of the absolute value comparison
    if (operand_x->sign == POSITIVE && operand_y->sign == POSITIVE) return who_is_big;
    if (operand_x->sign == NEGATIVE && operand_y->sign == NEGATIVE) return who_is_big * (-1);

    return 0;
}

/**
 * @brief Generates a random large integer with the specified sign and number of digits.
 * 
 * This function allocates memory for a new large integer specified by [bigint], sets its [sign],
 * and generates a random number of the specified [digit_num] size using rand() and masking 0xFF.
 * The memory is then refined to ensure the correct number of digits.
 * 
 * To generate a random number, we use rand() and mask each byte with 0xFF. For example, if rand()
 * returns 0xEF, 0xBE, 0xAD, 0xDE, then we obtain a random word number: 0xDEADBEEF.
 * 
 * @param bigint [output] The generated random large integer.
 * @param sign [input] Sign of the large integer.
 * @param digit_num [input] Number of digits for the large integer.
 */
void bigint_generate_random_number(Bigint** bigint, Sign sign, Word digit_num)
{
    /* Allocate Bigint */
    bigint_new(bigint, digit_num);
    (*bigint)->sign = sign;

    /* Variables for generating a random number */
    uint8_t* byte_rand_num = (uint8_t*)(*bigint)->digits; // pointer about bigint.
    int count = digit_num * SIZE_OF_WORD;                 // bytelen of bigint.

    /* 8-bit random number generator */
    while (count--)
        *byte_rand_num++ = (Word)rand() & MASK8BIT;

    /* Free unuse memory */
    bigint_refine(*bigint);
}

char bigint_is_zero(const Bigint* bigint)
{
    return (bigint->digit_num != 1 || bigint->digits[0] != 0) ? FALSE : TRUE;
}

char bigint_is_one(const Bigint* bigint)
{
    return (bigint->digit_num != 1 || bigint->digits[0] != 1) ? FALSE : TRUE;
}

void bigint_set_zero(Bigint** bigint)
{
    bigint_new(bigint, 1);

    (*bigint)->sign = POSITIVE;
    (*bigint)->digits[0] = 0;
}

void bigint_set_one(Bigint** bigint)
{
    bigint_new(bigint, 1);

    (*bigint)->sign = POSITIVE;
    (*bigint)->digits[0] = 1;
}

/**
 * @brief Expands the size of a large integer by adding leading zero words.
 * 
 * It seems like left shift algorithm but the differences is that this function add nunber of digits.
 * For example, if you have FF FF, and expand 2 words, then you have FF FF 00 00.
 * 
 * @param result [output] The expanded large integer.
 * @param bigint [input] The source large integer.
 * @param wordlen [input] Words to add for expansion.
 */
void bigint_expand(Bigint** result, const Bigint* bigint, Word wordlen) 
{
    /* No need to expand */
    if (wordlen == 0) {
        bigint_copy(result, bigint);
        return;
    }

    Bigint* tmp_result = NULL;
    Word count = bigint->digit_num;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num + wordlen);

    /* Expand(left shift) wordlen index */
    while(count--)
        tmp_result->digits[count + wordlen] = bigint->digits[count];

    /* Get Result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Compresses the size of a large integer by removing leading words.
 * 
 * It's exactly the same as right shift algorithm.
 * For example, if you have FF FF FF FF and compress 2 words, then you have FF FF.
 * 
 * @param result [output] The compressed large integer.
 * @param bigint [input] The source large integer.
 * @param wordlen [input] Number of leading words to remove.
 */
void bigint_compress(Bigint** result, const Bigint* bigint, Word wordlen) 
{
    /* Over compress */
    if (bigint->digit_num <= wordlen) {
        bigint_set_zero(result);
        return;
    }

    /* No need to compress */
    if (wordlen == 0) {
        bigint_copy(result, bigint);
        return;
    }

    Bigint* tmp_result = NULL;
    Word count = bigint->digit_num - wordlen;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num);

    /* Compress(shift right) wordlen index */
    while(count--)
        tmp_result->digits[count] = bigint->digits[count + wordlen];

    /* Get result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/* get bit length of bigint */
Word bigint_get_bit_length(const Bigint* bigint)
{
    return bigint->digit_num * BITLEN_OF_WORD;
}

/* get digits bit located in the bit_idx of an bigint */
Word bigint_get_bit(const Bigint* bigint, Word bit_idx)
{
    Word digit_idx = bit_idx / BITLEN_OF_WORD;
    Word bit_idx_in_one_digit = bit_idx % BITLEN_OF_WORD;

    /* is bit 1? or 0? */
    Word bit = (bigint->digits[digit_idx] >> bit_idx_in_one_digit) & MASK1BIT; 

    return bit;
}

void bigint_expand_one_bit(Bigint** result, const Bigint* bigint)
{
    Word carry = 0;
    Bigint* tmp_result = NULL;

    bigint_new(&tmp_result, bigint->digit_num + 1);

    for (int idx = 0; idx < bigint->digit_num; idx++) {
        tmp_result->digits[idx] = (bigint->digits[idx] << 1) + carry;
        carry = bigint->digits[idx] >> (BITLEN_OF_WORD - 1);
    }

    tmp_result->digits[bigint->digit_num] = carry;

    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);
    bigint_delete(&tmp_result);
}

void bigint_compress_one_bit(Bigint** result, const Bigint* bigint)
{
    Word carry = 0;
    Bigint* tmp_result = NULL;

    bigint_new(&tmp_result, bigint->digit_num);

    for (int idx = bigint->digit_num - 1; idx >= 0; idx--) {
        tmp_result->digits[idx] = (bigint->digits[idx] >> 1) + carry;
        carry = (bigint->digits[idx] & MASK1BIT) << (BITLEN_OF_WORD - 1);
    }

    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);
    bigint_delete(&tmp_result);
}