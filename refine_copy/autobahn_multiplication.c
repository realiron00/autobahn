#include "autobahn.h"

void debug_print(const Bigint* bigint)
{
    for(int i = bigint->digit_num - 1; i >= 0; i--) printf("%02x ", bigint->digits[i]);
    printf("\n");
}

/**
 * @brief Performs multiplication of two words using the schoolbook multiplication algorithm.
 * 
 * This function multiplies the values of [operand_x] and [operand_y] using the schoolbook multiplication
 * algorithm and stores the result in [result]. The algorithm is explained in the ASCII art above the function.
 * 
 *                      x_high || x_low     -- operand_x : word
 *                      y_high || y_low     -- operand_y : word
 *     ---------------------------------------------------------
 *                      [x_low * y_low]     -- mid0
 *              [x_high * y_low]            -- mid1           
 *              [y_high * x_low]            -- mid2    
 *      [x_high * y_high]                   -- mid3
 *     ---------------------------------------------------------
 *      [ result_high ]  [ result_low ]
 *             [ result_middle ]
 * 
 * @param result [output] Pointer to the variable to store the result of the multiplication.
 * @param operand_x [input] First word for multiplicatㄴion.
 * @param operand_y [input] Second word for multiplication.
 */
void word_multiplication(Bigint** result, const Word operand_x, const Word operand_y) 
{
    /* Initialize variables */
    Word bitlen_half = BITLEN_OF_WORD / 2;
    Word x_high = operand_x >> bitlen_half;            // Upper half of operand_x
    Word x_low  = (x_high << bitlen_half) ^ operand_x; // Lower half of operand_x
    Word y_high = operand_y >> bitlen_half;            // Upper half of operand_y
    Word y_low  = (y_high << bitlen_half) ^ operand_y; // Lower half of operand_y

    /* Multiplication : read function comment for detail */
    Word mid0 = x_high * y_low;
    Word mid1 = x_low * y_high;
    Word mid2 = x_low * y_low;
    Word mid3 = x_high * y_high;
    Word result_middle = mid0 + mid1;
    Word result_low = mid2 + (result_middle << bitlen_half);
    Word result_high = mid3 + (result_middle >> bitlen_half) + ((result_middle < mid0) << bitlen_half) + (result_low < mid2);

    /* Store the result */
    (*result)->digits[0] = result_low; // Store the lower half
    (*result)->digits[1] = result_high; // Store the upper half
}

/**
 * @brief Performs multiplication of two large integers using the textbook algorithm.
 * 
 * This function multiplies the values of [operand_x] and [operand_y] using the textbook algorithm
 * and stores the result in [result].
 * 
 * @param result [output] Pointer to the variable to store the result of the multiplication.
 * @param operand_x [input] Pointer to the first large integer for multiplication.
 * @param operand_y [input] Pointer to the second large integer for multiplication.
 */
void bigint_multiplication_textbook(Bigint** result, const Bigint* operand_x, const Bigint* operand_y) 
{
    if(bigint_is_zero(operand_x) || bigint_is_zero(operand_y)) {
        bigint_set_zero(result);
        return;
    }

    Bigint* tmp_result = NULL;
    Bigint* word_mult = NULL; // result of word multiplication
    Bigint* word_mult_expanded = NULL; // expanded result of word multiplication to set correct index

    /* Allocate */
    bigint_new(&tmp_result, operand_x->digit_num + operand_y->digit_num);
    bigint_new(&word_mult, 2);
    bigint_new(&word_mult_expanded, 1);

    /* Multiplication: O(n^2) */
    for (Word idx_x = 0; idx_x < operand_x->digit_num; idx_x++) 
    {
        for (Word idx_y = 0; idx_y < operand_y->digit_num; idx_y++) 
        {
            word_multiplication(&word_mult, operand_x->digits[idx_x], operand_y->digits[idx_y]); // Multiplication
            bigint_expand(&word_mult_expanded, word_mult, idx_x + idx_y); // set correct index
            bigint_addition(&tmp_result, tmp_result, word_mult_expanded); // Addition to the result
        }
    }

    /* Get the result */
    bigint_refine(tmp_result);
    tmp_result->sign = (operand_x->sign == operand_y->sign) ? POSITIVE : NEGATIVE;
    bigint_copy(result, tmp_result);

    /* Free */
    bigint_delete(&tmp_result);
    bigint_delete(&word_mult);
    bigint_delete(&word_mult_expanded);
}

void bigint_multiplication_karatsuba(Bigint** result, const Bigint* operand_x, const Bigint* operand_y) 
{
    if(bigint_is_zero(operand_x) || bigint_is_zero(operand_y)) {
        bigint_set_zero(result);
        return;
    }

    Word digit_num_min = operand_x->digit_num < operand_y->digit_num ? operand_x->digit_num : operand_y->digit_num;
	Word digit_num_max = operand_x->digit_num > operand_y->digit_num ? operand_x->digit_num : operand_y->digit_num;
	Word digit_num_half = (digit_num_max + 1) >> 1;
    
    // if (operand_x->digit_num == 1 || operand_y->digit_num == 1) {
    //     bigint_multiplication_textbook(result, operand_x, operand_y);
    //     return;
    // }

    /* Recursion stop flag */
    if (digit_num_min <= 2) {
        bigint_multiplication_textbook(result, operand_x, operand_y);
        return;
    }

	Bigint *x_low = NULL;
    Bigint *x_high = NULL;
    Bigint *y_low = NULL;
    Bigint *y_high = NULL;
    Bigint *y_lowhigh = NULL;
    Bigint *x_lowhigh = NULL;
    Bigint *result_low = NULL;
    Bigint *result_high = NULL;
    Bigint *result_middle = NULL;
    Bigint *tmp_result = NULL;
    Bigint* tmp_x = NULL;
    Bigint* tmp_y = NULL;

	bigint_new(&x_low, 1);
	bigint_new(&x_high, 1);
	bigint_new(&y_low, 1);
	bigint_new(&y_high, 1);
	bigint_new(&y_lowhigh, 1);
	bigint_new(&x_lowhigh, 1);
	bigint_new(&result_low, 1);
	bigint_new(&result_high, 1);
	bigint_new(&result_middle, 1);
    bigint_new(&tmp_x, 1);
    bigint_new(&tmp_y, 1);
	bigint_new(&tmp_result, 1);

    bigint_copy(&tmp_x, operand_x);
    bigint_copy(&tmp_y, operand_y);

    /* Make operands have the same number of digits */
    tmp_x->digits = (Word*)realloc(tmp_x->digits, digit_num_half * 2 * SIZE_OF_WORD);
    tmp_x->digit_num = digit_num_half * 2;
    tmp_y->digits = (Word*)realloc(tmp_y->digits, digit_num_half * 2 * SIZE_OF_WORD);
    tmp_y->digit_num = digit_num_half * 2;
	
    /* divide operand to upper and lower */
    bigint_copy_part(&x_low, tmp_x, 0, digit_num_half);
    bigint_copy_part(&y_low, tmp_y, 0, digit_num_half);
    bigint_copy_part(&x_high, tmp_x, digit_num_half, digit_num_half * 2);
    bigint_copy_part(&y_high, tmp_y, digit_num_half, digit_num_half * 2);

    /* compute high value and low value */
	bigint_multiplication_karatsuba(&result_high, x_high, y_high);
	bigint_multiplication_karatsuba(&result_low, x_low, y_low);

    /* compute middle value */
	bigint_subtraction(&x_lowhigh, x_high, x_low);
	bigint_subtraction(&y_lowhigh, y_low, y_high);
	bigint_multiplication_karatsuba(&result_middle, x_lowhigh, y_lowhigh);
    //result_middle->sign = x_lowhigh->sign ^ y_lowhigh->sign;
	bigint_addition(&result_middle, result_middle, result_high);
	bigint_addition(&result_middle, result_middle, result_low);

    /* compute result */
    bigint_expand(&result_high, result_high, digit_num_half * 2);
	bigint_expand(&result_middle, result_middle, digit_num_half);
	bigint_addition(&tmp_result, result_high, result_low);
	bigint_addition(&tmp_result, tmp_result, result_middle);

    /* End */  
	bigint_refine(tmp_result);
    tmp_result->sign = (tmp_x->sign == tmp_y->sign) ? POSITIVE : NEGATIVE;
	bigint_copy(result, tmp_result);

    /* free */
	bigint_delete(&x_low);
	bigint_delete(&x_high);
	bigint_delete(&y_low);
	bigint_delete(&y_high);
	bigint_delete(&result_low);
	bigint_delete(&result_high);
	bigint_delete(&result_middle);
	bigint_delete(&x_lowhigh);
	bigint_delete(&y_lowhigh);
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);
	bigint_delete(&tmp_result);	
};