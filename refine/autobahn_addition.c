#include "autobahn.h"

/**
 * @brief Performs addition of two words with a carry and determines the next carry.
 * 
 * This function adds the values of [operand_x] and [operand_y] along with the [current_carry].
 * The result is stored in [result], and the function sets [next_carry] if overflow occurs.
 * 
 * carry condition : let r = x + y. if r < x(or y) then carry is set.
 * 
 * @param result [output] Result of the addition.
 * @param next_carry [output] Carry from the current addition.
 * @param current_carry [input] Carry from the previous addition.
 * @param operand_x [input] First operand of the addition.
 * @param operand_y [input] Second operand of the addition.
 */
static void word_addition_with_carry(Word* result, Word* next_carry, Word current_carry, Word operand_x, Word operand_y) 
{
    *result     = operand_x + operand_y;        // r = x + y
    *next_carry = (*result < operand_x);        // set carry
    *result     += current_carry;               // r = x + y + c
    *next_carry += (*result < current_carry);   // set carry
}

/**
 * @brief Performs unsigned addition of two large integers.
 * 
 * This function adds the values of [operand_x] and [operand_y] and stores the result in [result].
 * It is assumed that [operand_x] is larger than or equal to [operand_y].
 * 
 * @param result [output] Result of the addition.
 * @param operand_x [input] First operand of the addition.
 * @param operand_y [input] Second operand of the addition.
 */
void bigint_addition_unsigned(Bigint** result, Bigint* operand_x, Bigint* operand_y)
{
    Word current_carry = 0;
    Word next_carry = 0;
    Bigint* tmp_result = NULL;  // This will be the result.
    


    /* Make operands have the same number of digits */  // !!         
    if (operand_y->digit_num != operand_x->digit_num)
    {
        Word previous_digit_num = operand_y->digit_num; // !!     
        operand_y->digits = (Word*)realloc(operand_y->digits, operand_x->digit_num * SIZE_OF_WORD);
        operand_y->digit_num = operand_x->digit_num;

        for (int i = operand_y->digit_num; i > previous_digit_num; i--) // !!
            operand_y->digits[i - 1] = 0;
    }


    /* Allocate Bigint */
    bigint_new(&tmp_result, operand_x->digit_num + 1);

    /* Addition word by word */
    for (size_t idx = 0; idx < operand_x->digit_num; idx++) {
        word_addition_with_carry(&(tmp_result->digits[idx]), &next_carry, current_carry, operand_x->digits[idx], operand_y->digits[idx]);
        current_carry = next_carry; // Update the carry.
    }
    
    /* Set the final carry */
    tmp_result->digits[operand_x->digit_num] = current_carry;

    /* Get result */
    bigint_refine(operand_y);
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Performs addition of two large integers.
 * 
 * This function adds the values of [operand_x] and [operand_y] and stores the result in [result].
 * 
 * @param result [output] Result of the addition.
 * @param operand_x [input] First operand of the addition.
 * @param operand_y [input] Second operand of the addition.
 */
void bigint_addition(Bigint** result, const Bigint* operand_x, const Bigint* operand_y) 
{
    /* Use temporary variables to avoid modifying the input operands */
    Bigint* tmp_x = NULL;
    Bigint* tmp_y = NULL;

    /* Allocate Bigint */
    bigint_new(&tmp_x, operand_x->digit_num);
    bigint_new(&tmp_y, operand_y->digit_num);

    /* Input Operands */
    bigint_copy(&tmp_x, operand_x);
    bigint_copy(&tmp_y, operand_y);

    /* Case: result = operand_x + (-operand_y) */
    if ((tmp_x->sign == POSITIVE && tmp_y->sign == NEGATIVE)) {
        tmp_y->sign = POSITIVE;
        bigint_subtraction(result, tmp_x, tmp_y);
        goto RETURN;
    }
    
    /* Case: result = (-operand_x) + operand_y */
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == POSITIVE) {
        tmp_x->sign = POSITIVE;
        bigint_subtraction(result, tmp_y, tmp_x);
        goto RETURN;
    }

    /* Perform addition */
    if (bigint_compare_abs(tmp_x, tmp_y) == 1)
        bigint_addition_unsigned(result, tmp_x, tmp_y);
    else
        bigint_addition_unsigned(result, tmp_y, tmp_x);
    
    /* Set the sign of the result */
    if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = POSITIVE;
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = NEGATIVE;

    RETURN:

    /* Free Bigint */
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);

    return;
}
