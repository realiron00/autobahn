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
void word2_long_div(Word* quotient, Word word_dividend_1, Word word_dividend_0, Word word_divisor)
{
    /* one block of quotient */
    Word word_quotient = 0;

    /* Allocate Bigint */
    Bigint* tmp_quotient = NULL;        // result of quotient
    Bigint* tmp_remainder = NULL;       // result of remainder
    Bigint* tmp_twobitlength = NULL;    // 2^w
    Bigint* tmp_dividend_bit = NULL;    // bit of dividend
    Bigint* tmp_divisor = NULL;         // word_divisor
    bigint_new(&tmp_quotient, 1);
    bigint_new(&tmp_remainder, 2);
    bigint_new(&tmp_twobitlength, 1);
    bigint_new(&tmp_dividend_bit, 1);
    bigint_new(&tmp_divisor, 1);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);                 // Q <- 0
    tmp_remainder->digits[0] = word_dividend_1;     // R <- A_1
    tmp_divisor->digits[0] = word_divisor;          // divisor

    /* Calculate 2^w */
    bigint_set_one(&tmp_twobitlength);
    for(int i = 0; i < BITLEN_OF_WORD; i++)
        bigint_expand_one_bit(&tmp_twobitlength, tmp_twobitlength); // 2^w

    /* 2-word long division */
    for(int j = BITLEN_OF_WORD - 1; j >= 0; j--)
    {
        bigint_refine(tmp_remainder); // refine remainder

        /* Case R >= 2^w−1 */
        if(bigint_compare(tmp_remainder, tmp_twobitlength) == LEFT_IS_BIG) //! 크거나 같아야함(사실 여기서는 이럴 일 없긴 함)
        {
            word_quotient += 1 << j; // Q <- Q + 2^j
            
            bigint_expand_one_bit(&tmp_remainder, tmp_remainder);               // R <- 2R
            tmp_dividend_bit->digits[0] = ((word_dividend_0 & (1 << j)) >> j);  // a_j
            
            bigint_addition(&tmp_remainder, tmp_remainder, tmp_dividend_bit);   // R <- 2R + a_j
            bigint_subtraction(&tmp_remainder, tmp_remainder, tmp_divisor);     // R <- 2R + a_j - B //!헷갈릴 수 있음

            tmp_quotient->digits[0] = word_quotient; // Q
        }
        /* Case R < 2^w−1 */
        else
        {
            bigint_expand_one_bit(&tmp_remainder, tmp_remainder);              // R <- 2R
            tmp_dividend_bit->digits[0] = ((word_dividend_0 & (1 << j)) >> j); // a_j
            
            bigint_addition(&tmp_remainder, tmp_remainder, tmp_dividend_bit); // R <- 2R + a_j

            if(bigint_compare(tmp_remainder, tmp_divisor) >= 0) // R >= B
            {
                word_quotient += 1 << j; // Q <- Q + 2^j

                bigint_subtraction(&tmp_remainder, tmp_remainder, tmp_divisor); // R <- R - B
                
                tmp_quotient->digits[0] = word_quotient; // Q
            }
        }
    }

    *quotient = word_quotient;  // Q <- word_quotient

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&tmp_twobitlength);
    bigint_delete(&tmp_dividend_bit);
    bigint_delete(&tmp_divisor);
}

/*
DIVCC(A, B)
Input: A = A_0 + A_1*W^1 + ... + A_n-1*W^n-1, B = B_0 + B_1*W^1 + ... + B_m-1*W^m-1 
(A_j , B_j ∈ [0, W), 0 < B ≤ A < BW, B_m−1 ≥ 2^w−1)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B, Q ∈ [0, W)).
1: procedure DIVCC(A, B)
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
//! 느낌상 2word_long_div에 Bigint를 적용시키는게 맞는거 같음
void bi_divcc(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* one block of quotient */
    Word word_quotient = 0; // one block of quotient

    /* Allocate Bigint */
    Bigint* tmp_quotient = NULL;    //result of quotient
    Bigint* tmp_remainder = NULL;   //result of remainder
    Bigint* tmp_bq = NULL;          //result of BQ
    Bigint* tmp_one = NULL;         //1
    bigint_new(&tmp_quotient, 1);
    bigint_new(&tmp_remainder, divisor->digit_num);
    bigint_new(&tmp_bq, divisor->digit_num + 1);
    bigint_new(&tmp_one, 1);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);
    bigint_set_one(&tmp_one);

    /* Case dividend->digit_num(=n) == divisor->digit_num(=m) */
    if(dividend->digit_num == divisor->digit_num)
    {
        word_quotient = dividend->digits[divisor->digit_num - 1] / divisor->digits[divisor->digit_num - 1];
        tmp_quotient->digits[0] = word_quotient; // Q <- |A_(m - 1) / B_(m - 1)|
    }
    /* Case dividend->digit_num(=n) != divisor->digit_num(=m) */
    else
    {
        /* Case A_m = B_m−1 */
        if(dividend->digits[divisor->digit_num] == divisor->digits[divisor->digit_num - 1])
        {
            tmp_quotient->digits[0] = 0xFFFFFFFF; // Q <- W - 1 //! 32비트에서만 성공, 다른거를 위한 수정 필요
        }
        /* Case A_m < B_(m-1) */
        else if(dividend->digits[divisor->digit_num]<divisor->digits[divisor->digit_num - 1])
        {
            Word word_dividend_1 = dividend->digits[divisor->digit_num];        // A_m
            Word word_dividend_0 = dividend->digits[divisor->digit_num - 1];    // A_(m-1)
            Word word_divisor = divisor->digits[divisor->digit_num - 1];        // B_(m-1)

            word2_long_div(&word_quotient, word_dividend_1, word_dividend_0, word_divisor); // 2word_long_div(A_mW + A_m−1, B_m−1)
            tmp_quotient->digits[0] = word_quotient; // Q <- |A_mW + A_m−1 / B_m−1|
        }
    }

    /* Calculate quotient and remainder */
    bigint_multiplication_textbook(&tmp_bq, tmp_quotient, divisor); // BQ
    bigint_subtraction(&tmp_remainder, dividend, tmp_bq);         // R <- A - BQ

    while(tmp_remainder->sign == NEGATIVE) // While R < 0
    {
        bigint_subtraction(&tmp_quotient, tmp_quotient, tmp_one);   // Q <- Q - 1
        bigint_addition(&tmp_remainder, tmp_remainder, divisor);    // R <- R + B
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&tmp_bq);
    bigint_delete(&tmp_one);
}

/*
DIVC(a, b)
input: A = A_0 + A_1*W^1 + ... + A_m*W^m, B = B_0 + B_1*W^1 + ... + B_m-1*W^m-1 (A_j, B_j ∈ [0, W), 0 ≤ A < BW)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B, Q ∈ [0, W)).
1: procedure DIVC(A, B)
2: if A < B then
3:  return (0, A)
4: end if
5: Compute k ∈ Z≥0 such that 2^k*B_m−1 ∈ [2^w−1, 2^w)
6: A', B' ← 2^k*A, 2^k*B
7: Q', R' ← DIVCC(A', B')

8: Q, R ← Q', 2^−k*R'
9: return (Q, R)
10: end procedure
*/
void bi_divc(Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor)
{
    /* Allocate Bigint */
    Bigint* power_of_two = NULL;        // 2^k //!뒤에 무슨 말을 쓸지 고민
    Bigint* dividend_prime = NULL;      // 2^k*A
    Bigint* divisor_prime = NULL;       // 2^k*B
    Bigint* quotient_prime = NULL;      // Q'
    Bigint* remainder_prime = NULL;     // R'
    bigint_new(&power_of_two, 2);
    bigint_new(&dividend_prime, dividend->digit_num + 1);
    bigint_new(&divisor_prime, divisor->digit_num + 1);
    bigint_new(&quotient_prime, dividend->digit_num + 1);
    bigint_new(&remainder_prime, divisor->digit_num + 1);
    
    /* Special case : dividend < divisor, we have zero quotient and dividend remainder*/
    if(bigint_compare(dividend, divisor) == LEFT_IS_SMALL)
    {
        bigint_set_zero(quotient);
        bigint_copy(remainder, dividend);
        return;
    }

    /* Compute k ∈ Z≥0 such that 2^k*B_(m−1) ∈ [2^(w−1), 2^w) */
    Word k = 0;
    Word temp = divisor->digits[divisor->digit_num - 1];

    while(1) //! 일단 32비트는 성공, 다른거를 위한 수정 필요
    {
        if(temp >= 0x80000000)
            break;
        
        temp <<= 1;
        k++;
    }

    bigint_set_one(&power_of_two); // init to compute 2^k

    for(int i = 0; i < k; i++)
        bigint_expand_one_bit(&power_of_two, power_of_two); // 2^k

    /*compute 2^k*A, 2^k*B */
    bigint_multiplication_textbook(&dividend_prime, dividend, power_of_two);    // A' <- 2^k*A
    bigint_multiplication_textbook(&divisor_prime, divisor, power_of_two);      // B' <- 2^k*B
    bigint_refine(dividend_prime);
    bigint_refine(divisor_prime);

    /* DIVCC(A', B') */
    bi_divcc(&quotient_prime, &remainder_prime, dividend_prime, divisor_prime); // Q', R' <- DIVCC(A', B')

    /* Q, R <- Q', 2^−k*R' */
    for(int i = 0; i < k; i++)
        bigint_compress_one_bit(&remainder_prime, remainder_prime); // 2^−k*R'

    /* Get result */
    bigint_refine(quotient_prime);
    bigint_refine(remainder_prime);
    bigint_copy(quotient, quotient_prime);
    bigint_copy(remainder, remainder_prime);

    /* Free Bigint */
    bigint_delete(&power_of_two);
    bigint_delete(&dividend_prime);
    bigint_delete(&divisor_prime);
    bigint_delete(&quotient_prime);
    bigint_delete(&remainder_prime);
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
    Bigint* tmp_quotient = NULL;        // result of quotient
    Bigint* tmp_remainder = NULL;       // result of remainder
    Bigint* tmp_quotient_digit = NULL;  // result of quotient digit
    Bigint* tmp_rwa = NULL;             // result of remainder with addition
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&tmp_quotient_digit, 1);
    bigint_new(&tmp_rwa, size_remainder + 1);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* general long division */
    for(int i = dividend->digit_num - 1; i >= 0; i--)
    {
        Bigint* tmp_dividend = NULL; // result of dividend one block
        bigint_new(&tmp_dividend, 1);
        tmp_dividend->digits[0] = dividend->digits[i]; // A_i

        //rwa = tmp_remainder*W + dividend->digits[i]
        bigint_expand(&tmp_rwa, tmp_remainder, 1);          //rwa <- R_(i+1)*W
        bigint_addition(&tmp_rwa, tmp_rwa, tmp_dividend);   //rwa <- R_(i+1)*W + A_i

        bi_divc(&tmp_quotient_digit, &tmp_remainder, tmp_rwa, divisor); //Q_i, R_i <- DIVC(R_(i+1)*W + A_i, B)
        bigint_expand(&tmp_quotient, tmp_quotient, 1); //Q_i
        bigint_addition(&tmp_quotient, tmp_quotient, tmp_quotient_digit); //Q <- Q + Q_i*W^i

        bigint_delete(&tmp_dividend); //free tmp_dividend
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&tmp_quotient_digit);
    bigint_delete(&tmp_rwa);
}