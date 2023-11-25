#include "autobahn.h"

/**
 * @brief Checks for invalid or special cases of division.
 * 
 * Checks for the following cases:
 * - Invalid Case 1: Divisor is zero.
 * - Invalid Case 2: Either divisor or dividend is negative.
 * - Special Case 1: Dividend is less than divisor.
 * - Special Case 2: Divisor is one.
 * 
 * @param quotient [output] The result of the quotient.
 * @param remainder [output] The result of the remainder.
 * @param dividend [input] Dividend large integer.
 * @param divisor [input] Divisor large integer.
 */
static bool bigint_division_special_case(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* Invalid case: zero divisor */
    if(bigint_is_zero(divisor) == TRUE) {
        printf("divisor must be non-zero\n"); // error log
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Invalid case: operands are negative */
    if(divisor->sign == NEGATIVE || dividend->sign == NEGATIVE) {
        printf("divisor and dividend must be positive\n"); // error log
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Special case : dividend < divisor, we have zero quotient and dividend remainder */
    if(bigint_compare(dividend, divisor) == LEFT_IS_SMALL) 
    {
        bigint_set_zero(quotient);
        bigint_copy(remainder, dividend);
        return true;
    }
    
    /* Special case : divisor is one, we have zero remainder and dividend quotient */
    if(bigint_is_one(divisor) == TRUE) 
    {
        bigint_copy(quotient, dividend);
        bigint_set_zero(remainder);
        return true;
    }

    /* dividing is needed */
    return false;
}

/**
 * @brief Performs binary long division algorithm.
 * 
 * Divides the large integer [dividend] by [divisor] using the binary long division algorithm.
 * The result is stored in [quotient], and the remainder is stored in [remainder].
 * The formula is given by: B = AQ + R (where 0 <= R < B)
 * 
 * @param quotient [output] The result of the quotient.
 * @param remainder [output] The result of the remainder.
 * @param dividend [input] Dividend large integer.
 * @param divisor [input] Divisor large integer.
 */
void bigint_division_binary_long(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true) return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint* tmp_quotient = NULL;  // result of quotient
    Bigint* tmp_remainder = NULL; // result of remainder
    Bigint* power_of_two = NULL;  // 2^i, we will use this for get quotient
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&power_of_two, size_quotient);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* iteration count */
    Word dividend_bit_len = bigint_get_bit_length(dividend);

    /* Binary long division */
    while(dividend_bit_len--)
    {
        bigint_expand_one_bit(&tmp_remainder, tmp_remainder);                   // R <- 2R
        tmp_remainder->digits[0] += bigint_get_bit(dividend, dividend_bit_len); // R <- 2R + bit of dividend

        if(bigint_compare(tmp_remainder, divisor) == LEFT_IS_SMALL) // R < B then do not R - B.
            continue;                                               
                                                                    
        bigint_set_one(&power_of_two);                // init to compute 2^i
                                                                    
        for(int i = 0; i < dividend_bit_len; i++)               
            bigint_expand_one_bit(&power_of_two, power_of_two);     // 2^i
                                                                    
        bigint_addition(&tmp_quotient, tmp_quotient, power_of_two); // Q <- Q + 2^i
        bigint_subtraction(&tmp_remainder, tmp_remainder, divisor); // R <- R - B
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&power_of_two);
}

/**
 * @brief Performs naive division algorithm.
 * 
 * Divides the large integer [dividend] by [divisor] using the naive division algorithm.
 * The result is stored in [quotient], and the remainder is stored in [remainder].
 * The formula is given by: B = AQ + R (where 0 <= R < B)
 * 
 * Note: This algorithm is very slow. Do not use it.
 * 
 * @param quotient [output] The result of the quotient.
 * @param remainder [output] The result of the remainder.
 * @param dividend [input] Dividend large integer.
 * @param divisor [input] Divisor large integer.
 */
void bi_division_naive(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true) return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint* tmp_quotient = NULL;  // result of quotient
    Bigint* tmp_remainder = NULL; // result of remainder
    Bigint* one = NULL;           // 1
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&one, 1);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_copy(&tmp_remainder, dividend);
    bigint_set_one(&one);

    /* Naive division */
    while(bigint_compare(tmp_remainder, divisor) != LEFT_IS_SMALL)
    {
        bigint_addition(&tmp_quotient, tmp_quotient, one);            // Q <- Q + 1
        bigint_subtraction(&tmp_remainder, tmp_remainder, divisor);   // R <- R - B
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient,tmp_quotient);
    bigint_copy(remainder,tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&one);
}

/*
Multi-Precision Long Division
A = A_0 + A_1*W^1 + ... + A_n-1*W^n-1, B (A_j ∈ [0, W))
Output: (Q = Q_0 + Q_1*W^1 + ... + Q_n-1*W^n-1, R)
such that A = BQ + R (0 ≤ R < B, Q_j ∈ [0, W)).
1: procedure DIVlong(A, B)
2: if A < B then
3: return (0, A)
4: end if
5: R_n ← 0
6: for i = n − 1 downto 0 do
7: (Q_i, R_i) ← DIVC(R_i+1*W + A_i, B). 
8: end for
9: Q ← Q_0 + Q_1*W + ... + Q_n−1*W^n−1
10: return (Q, R0)
11: end procedure
*/
void bigint_division_general_long(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true) return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;
    
    /* Allocate Bigint */
    Bigint* tmp_quotient = NULL;  // result of quotient
    Bigint* tmp_remainder = NULL; // result of remainder
    Bigint* tmp_quotient_digit = NULL;  // result of quotient digit
    Bigint* tmp_rwa = NULL;  // result of remainder with addition
    // (rwa->digits[0] = remainder->digits[i], rwa->digits[1] = tmp_remainder->digits[i+1])
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&tmp_quotient_digit, 1);

}