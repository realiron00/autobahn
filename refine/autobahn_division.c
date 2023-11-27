#include "autobahn.h"

#define GET_MSB(word) ((word) >> (BITLEN_OF_WORD - 1))
#define GET_BIT(word, bit_idx) ((word) >> (bit_idx)) & 1

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
static bool bigint_division_special_case(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Invalid case: zero divisor */
    if (bigint_is_zero(divisor) == TRUE)
    {
        printf("divisor must be non-zero\n"); // error log
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Invalid case: operands are negative */
    if (divisor->sign == NEGATIVE || dividend->sign == NEGATIVE)
    {
        printf("divisor and dividend must be positive\n"); // error log
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Special case : dividend < divisor, we have zero quotient and dividend remainder */
    if (bigint_compare(dividend, divisor) == LEFT_IS_SMALL)
    {
        bigint_set_zero(quotient);
        bigint_copy(remainder, dividend);
        return true;
    }

    /* Special case : divisor is one, we have zero remainder and dividend quotient */
    if (bigint_is_one(divisor) == TRUE)
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
void bigint_division_binary_long(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true)
        return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint *tmp_quotient = NULL;  // result of quotient
    Bigint *tmp_remainder = NULL; // result of remainder
    Bigint *two_power = NULL;  // 2^i, we will use this for get quotient
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&two_power, size_quotient);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* iteration count */
    Word dividend_bit_len = bigint_get_bit_length(dividend);

    /* Binary long division */
    while (dividend_bit_len--)
    {
        bigint_expand_one_bit(&tmp_remainder, tmp_remainder);                   // R <- 2R
        tmp_remainder->digits[0] += bigint_get_bit(dividend, dividend_bit_len); // R <- 2R + bit of dividend

        if (bigint_compare(tmp_remainder, divisor) == LEFT_IS_SMALL) // R < B then do not R - B.
            continue;

        bigint_set_one(&two_power); // init to compute 2^i

        for (int i = 0; i < dividend_bit_len; i++)
            bigint_expand_one_bit(&two_power, two_power); // 2^i

        bigint_addition(&tmp_quotient, tmp_quotient, two_power); // Q <- Q + 2^i
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
    bigint_delete(&two_power);
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
void bigint_division_naive(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true)
        return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint *tmp_quotient = NULL;  // result of quotient
    Bigint *tmp_remainder = NULL; // result of remainder
    Bigint *one = NULL;           // 1
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&one, 1);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_copy(&tmp_remainder, dividend);
    bigint_set_one(&one);

    /* Naive division */
    while (bigint_compare(tmp_remainder, divisor) != LEFT_IS_SMALL)
    {
        bigint_addition(&tmp_quotient, tmp_quotient, one);          // Q <- Q + 1
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
    bigint_delete(&one);
}

/*
2-word long division
Input: A = A_1W + A_0, B
(A_1, A_0 ∈ [0, W), B ∈ [2^w−1, 2^w), A_1 < B, A0 = a_0 + a_1*2^1 + ... + a_w-1*2^w-1)
Output: Q such that A = BQ + R (0 ≤ R < B, Q_j ∈ [0, W)).
1: (Q, R) ← (0, A_1)
2: for j = w − 1 downto 0 do
3:  if R ≥ 2^w−1 then
4:      (Q, R) ← (Q + 2^j, 2R + a_j − B). 2R + a_j − B = R + a_j − (B − R) < W
5:  else
6:      R ← 2R + a_j. R ∈ [0, W)
7:      if R ≥ B then
8:          (Q, R) ← (Q + 2^j, R − B)
9:      end if
10: end if
11: end for
12: return Q
*/
static Word division_two_word(Word dividend_high, Word dividend_low, Word divisor)
{
    /* 파라메터 조건에 부합하는지 if문 넣기 */

    Word quotient = 0;
    Word remainder = dividend_high;

    for (int bit_idx = BITLEN_OF_WORD - 1; bit_idx >= 0; bit_idx--)
    {
        Word bit_dividend_low = GET_BIT(dividend_low, bit_idx); // GET_BIT: bit_idx 번째 비트 추출
        Word bit_expanded = 1 << bit_idx;

        if (GET_MSB(remainder) == 1)
        { // GET_MSB: remainder 의 최상위 비트 추출
            quotient += bit_expanded;
            remainder = remainder * 2 + bit_dividend_low - divisor;
        }
        else
        {
            remainder = remainder * 2 + bit_dividend_low;

            if (remainder >= divisor)
            {
                quotient += bit_expanded;
                remainder = remainder - divisor;
            }
        }
    }

    return quotient;
}

static void multiplication_bigint_and_word(Bigint **result, const Bigint *bigint, const Word word)
{
    Bigint *word_to_bigint = NULL;

    /* Allocate Bigint */
    bigint_new(&word_to_bigint, 1);

    /* word to bigint for compatible */
    word_to_bigint->digits[0] = word;

    /* multiplication between bigint and word */
    bigint_multiplication_textbook(result, bigint, word_to_bigint);

    /* Free Bigint */
    bigint_delete(&word_to_bigint);
}

static void addition_bigint_and_word(Bigint **result, const Bigint *bigint, const Word word)
{
    Bigint *word_to_bigint = NULL;

    /* Allocate Bigint */
    bigint_new(&word_to_bigint, 1);

    /* word to bigint for compatible */
    word_to_bigint->digits[0] = word;

    /* multiplication between bigint and word */
    bigint_addition(result, bigint, word_to_bigint);

    /* Free Bigint */
    bigint_delete(&word_to_bigint);
}

/*
division_cc(A, B)
Input: A = A_0 + A_1*W^1 + ... + A_n-1*W^n-1, B = B_0 + B_1*W^1 + ... + B_m-1*W^m-1
(A_j , B_j ∈ [0, W), 0 < B ≤ A < BW, B_m−1 ≥ 2^w−1)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B, Q ∈ [0, W)).
1: procedure division_cc(A, B)
2: if n = m then
3:  Q ← A_m-1/B_m-1 . Q ∈ [0, W)
4: end if
5: if n = m + 1 then
6:  if A_m = B_m−1 then
7:      Q ← W − 1
8:  else . Am < Bm−1
9:      Q ← 2word_long_div(A_mW + A_m−1, B_m−1) . Q ∈ [0, W)
10: end if
11: end if
12: R ← A − BQ . Q ∈ [0, W)
13: while R < 0 do
14:     (Q, R) ← (Q − 1, R + B) . At most 2 computations
15: end while
16: return (Q, R)
17: end procedure
*/
static void division_cc(Word *quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    Word word_dividend_high = dividend->digits[divisor->digit_num];
    Word word_dividend_low  = dividend->digits[divisor->digit_num - 1];
    Word word_divisor       =  divisor->digits[divisor->digit_num - 1];

    if (dividend->digit_num == divisor->digit_num)
        *quotient = word_dividend_low / word_divisor;

    if (dividend->digit_num == divisor->digit_num + 1)
    {
        if (word_dividend_high == word_divisor)
            *quotient = -1;
        else
            *quotient = division_two_word(word_dividend_high, word_dividend_low, word_divisor);
    }

    multiplication_bigint_and_word(remainder, divisor, *quotient); // R <- BQ
    bigint_subtraction(remainder, dividend, *remainder);        // R <- A - BQ

    while ((*remainder)->sign == NEGATIVE) // if (R < 0)
    {
        *quotient = *quotient - 1;                               // Q <- Q - 1
        bigint_addition(remainder, *remainder, divisor); // R <- R - B
    }
}

/*
division_c(a, b)
input: A = A_0 + A_1*W^1 + ... + A_m*W^m, B = B_0 + B_1*W^1 + ... + B_m-1*W^m-1 (A_j, B_j ∈ [0, W), 0 ≤ A < BW)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B, Q ∈ [0, W)).
1: procedure division_c(A, B)
2: if A < B then
3:  return (0, A)
4: end if
5: Compute k ∈ Z≥0 such that 2^k*B_m−1 ∈ [2^w−1, 2^w)
6: A', B' ← 2^k*A, 2^k*B
7: Q', R' ← division_cc(A', B')

8: Q, R ← Q', 2^−k*R'
9: return (Q, R)
10: end procedure
*/
void division_c(Word *quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Special Case : dividend < divisor */
    if (bigint_compare(dividend, divisor) == LEFT_IS_SMALL) {
        *quotient = 0;
        bigint_copy(remainder, dividend);
        return;
    }
    
    Word count_shift = 0;
    Word most_significant_word_of_divisor = divisor->digits[divisor->digit_num - 1];

    Bigint *dividend_expand = NULL;
    Bigint *divisor_expand = NULL;

    /* Allocate Bigint */
    bigint_new(&dividend_expand, dividend->digit_num);
    bigint_new(&divisor_expand, divisor->digit_num);

    /* init */
    bigint_copy(&dividend_expand, dividend);
    bigint_copy(&divisor_expand, divisor);

    /* expand */
    while (GET_MSB(most_significant_word_of_divisor) != 1)
    {
        bigint_expand_one_bit(&dividend_expand, dividend_expand);
        bigint_expand_one_bit(&divisor_expand, divisor_expand); 

        most_significant_word_of_divisor <<= 1;
        count_shift++;
    }

    /* div */
    division_cc(quotient, remainder, dividend_expand, divisor_expand);

    /* compress */
    while(count_shift--)
        bigint_compress_one_bit(remainder, *remainder);

    /* Free Bigint */
    bigint_delete(&dividend_expand);
    bigint_delete(&divisor_expand);
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
7: (Q_i, R_i) ← division_c(R_i+1*W + A_i, B).
8: end for
9: Q ← Q_0 + Q_1*W + ... + Q_n−1*W^n−1
10: return (Q, R0)
11: end procedure
*/
void bigint_division_word_long(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true) return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint *tmp_quotient = NULL;       // result of quotient
    Bigint *tmp_remainder = NULL;      // result of remainder
    Bigint *rw_plus_a = NULL;          // result of remainder with addition

    // (rwa->digits[0] = remainder->digits[i], rwa->digits[1] = tmp_remainder->digits[i+1])
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&rw_plus_a, size_remainder + 1);

    /* init */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* long div */
    for (int digit_idx = dividend->digit_num - 1; digit_idx >= 0; digit_idx--)
    {
        Word word_dividend = dividend->digits[digit_idx];
        Word word_quotient = 0;

        /* rwa = tmp_remainder*W + dividend->digits[digit_idx] */
        bigint_expand(&rw_plus_a, tmp_remainder, 1);
        addition_bigint_and_word(&rw_plus_a, rw_plus_a, word_dividend);

        division_c(&word_quotient, &tmp_remainder, rw_plus_a, divisor);
        bigint_expand(&tmp_quotient, tmp_quotient, 1);
        addition_bigint_and_word(&tmp_quotient, tmp_quotient, word_quotient);
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&rw_plus_a);
}