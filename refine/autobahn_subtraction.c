#include "autobahn.h"

/**
 * @brief Performs subtraction of two words with a borrow and determines the next borrow.
 * 
 * This function subtracts [operand_y] from [operand_x] along with the [current_borrow].
 * The result is stored in [result], and the function sets [next_borrow] if the result is negative.
 * 
 * borrow condition : let r = x - y. if x < y then borrow is set.
 * 
 * @param result [output] Result of the subtraction.
 * @param next_borrow [output] Borrow from the current subtraction.
 * @param current_borrow [input] Borrow from the previous subtraction.
 * @param operand_x [input] First operand of the subtraction.
 * @param operand_y [input] Second operand of the subtraction.
 */
static void word_subtraction_with_borrow(Word* result, Word* next_borrow, Word current_borrow, Word operand_x, Word operand_y) 
{
    *result      = operand_x - operand_y;       // r = x - y
    *next_borrow = operand_x < operand_y;       // set borrow
    *next_borrow += *result < current_borrow;   // set borrow
    *result      -= current_borrow;             // r = x - y - b
}

/**
 * @brief Performs unsigned subtraction of two large integers.
 * 
 * This function subtracts the value of [operand_y] from [operand_x] and stores the result in [result].
 * It is assumed that [operand_x] is larger than or equal to [operand_y].
 * 
 * @param result [output] Result of the subtraction.
 * @param operand_x [input] First operand of the subtraction.
 * @param operand_y [input] Second operand of the subtraction.
 */
void bigint_subtraction_unsigned(Bigint** result, Bigint* operand_x, Bigint* operand_y)
{
    Word current_borrow = 0;
    Word next_borrow = 0;
    Bigint* tmp_result = NULL;  // This will be the result.

    /* Make operands have the same number of digits */
    if (operand_x->digit_num != operand_y->digit_num) {
        operand_y->digits = (Word*)realloc(operand_y->digits, operand_x->digit_num * SIZE_OF_WORD);
        operand_y->digit_num = operand_x->digit_num;
    }

    /* Allocate Bigint */
    bigint_new(&tmp_result, operand_x->digit_num);

    /* Subtraction word by word */
    for (int idx = 0; idx < operand_x->digit_num; idx++) 
    {
        word_subtraction_with_borrow(&(tmp_result->digits[idx]), &next_borrow, current_borrow, operand_x->digits[idx], operand_y->digits[idx]);
        current_borrow = next_borrow; // Update the borrow.
    }

    /* Get result */
    bigint_refine(operand_y);
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Performs subtraction of two large integers.
 * 
 * This function subtracts the value of [operand_y] from [operand_x] and stores the result in [result].
 * 
 * @param result [output] Result of the subtraction.
 * @param operand_x [input] First operand of the subtraction.
 * @param operand_y [input] Second operand of the subtraction.
 */
void bigint_subtraction(Bigint** result, const Bigint* operand_x, const Bigint* operand_y)
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

    /* Case: result = operand_x - (-operand_y) */
    if (tmp_x->sign == POSITIVE && tmp_y->sign == NEGATIVE) {
        tmp_y->sign = POSITIVE;
        bigint_addition(result, tmp_x, tmp_y);
        (*result)->sign = POSITIVE;
        goto RETURN;
    }

    /* Case: result = (-operand_x) - operand_y */
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == POSITIVE) {
        tmp_x->sign = POSITIVE;
        bigint_addition(result, tmp_x, tmp_y);
        (*result)->sign = NEGATIVE;
        goto RETURN;
    }

    /* Case: operand_x == operand_y */
    if(bigint_compare(tmp_x, tmp_y) == SAME) {
        bigint_set_zero(result);
        goto RETURN;
    }

    /* Case: operand_x > operand_y */
    if (bigint_compare_abs(tmp_x, tmp_y) == LEFT_IS_BIG) 
    {
        bigint_subtraction_unsigned(result, tmp_x, tmp_y);
        
        /* Set the sign of the result */
        if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = POSITIVE;
	    if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = NEGATIVE;

        goto RETURN;
    }
    else
    {
        bigint_subtraction_unsigned(result, tmp_y, tmp_x);

        /* Set the sign of the result */
        if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = NEGATIVE;
	    if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = POSITIVE;

        goto RETURN;
    }

    RETURN:

    /* Free Bigint */
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);

    return;
}