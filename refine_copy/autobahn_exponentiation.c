#include "autobahn.h"

/**
 * @brief Exponentiation using the left-to-right method.
 * 
 * Calculates the result of exponentiation using the left-to-right method.
 * [Result] = [base]^[exponent].
 * Note that left-to-right method is fast than montgomery method, but not constant time.
 * 
 * @param result [output] Result of exponentiation.
 * @param base [input] The base value.
 * @param exponent [input] The exponent value.
 */
void bigint_exponentiation_left_to_right(Bigint** result, const Bigint* base, const Bigint* exponent)
{  
    /* base and exponent are not NEGATIVE */
    if(base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case : base or exponent must be positive.\n");
        return;
    }

    /* Allocate Bigint and initialize for algorithm */
    Bigint* result_tmp = NULL;
    bigint_new(&result_tmp, 1);
    bigint_set_one(&result_tmp);

    /* Iteration count */
    Word bitlen = bigint_get_bit_length(exponent);

    /* Left-to-right */
    while (bitlen--)
    {
        //bigint_squaring(&result_tmp, result_tmp); // always squaring.

        if (bigint_get_bit(exponent, bitlen) == 1)
            bigint_multiplication_textbook(&result_tmp, result_tmp, base); // conditional multiplication.
    }

    /* get result */
    bigint_refine(result_tmp);
    bigint_copy(result, result_tmp);

    /* Free Bigint */
    bigint_delete(&result_tmp);
}

/**
 * @brief Exponentiation using the montgomery ladder method.
 * 
 * Calculates the result of exponentiation using the montgomery method.
 * [Result] = [base]^[exponent].
 * Note that montgomery method is constant time.
 * You use this algorithm if you want secure algorithm.
 * 
 * @param result [output] Result of exponentiation.
 * @param base [input] The base value.
 * @param exponent [input] The exponent value.
 */
void bigint_exponentiation_montgomery_ladder(Bigint** result, const Bigint* base, const Bigint* exponent)
{  
    /* base and exponent are not NEGATIVE */
    if(base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case : base or exponent must be positive.\n");
        return;
    }

    /* Allocate Bigint and initailize for algorithm */
    Bigint* left = NULL; // result
    Bigint* right = NULL;
    bigint_set_one(&left); // left = 1
    bigint_copy(&right, base);           // right = x

    /* Iteration count */
    Word bitlen = bigint_get_bit_length(exponent);

    /* Montgomery Ladder */
    while (bitlen--)
    {
        if (bigint_get_bit(exponent, bitlen)) {
            bigint_multiplication_textbook(&left, left, right);  // always multiplication
            //bigint_squaring(&right, right);             // always squaring
        } 
        else {
            bigint_multiplication_textbook(&right, right, left); // note: result is in right. not left.
            //bigint_squaring(&left, left);               // note: result is int left. not right
        }
    }

    /* Get result */
    bigint_refine(left);
    bigint_copy(result, left);

    /* Free Bigint */
    bigint_delete(&left);
}

/**
 * @brief Exponentiation using the left-to-right method with modular reduction.
 * 
 * Calculates the result of exponentiation using the left-to-right method and reduces the result modulo [modular].
 * [Result] = [base]^[exponent] % [modular].
 * Note that the left-to-right method is faster than the Montgomery method but not constant time.
 * 
 * @param result [output] Result of modulo exponentiation.
 * @param base [input] The base value.
 * @param exponent [input] The exponent value.
 * @param modular [input] The modulus value.
 */
void bigint_exponentiation_modular_left_to_right(Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular)
{
    /* base and exponent are not NEGATIVE */
    if(base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case : base or exponent must be positive.\n");
        return;
    }

    /* Allocate Bigint and initialize for algorithm */
    Bigint* result_tmp = NULL;
    bigint_new(&result_tmp, 1);
    bigint_set_one(&result_tmp);

    /* Iteration count */
    Word bitlen = bigint_get_bit_length(exponent);

    /* Left-to-right */
    while (bitlen--)
    {
        //bigint_squaring(&result_tmp, result_tmp); // always squaring.
        //bigint_modular_barrett(&result_tmp, modular);        

        if (bigint_get_bit(exponent, bitlen) == 1)
            bigint_multiplication_textbook(&result_tmp, result_tmp, base); // conditional multiplication.
            //bigint_modular_barrett(&result_tmp, modular);        
    }

    /* get result */
    bigint_refine(result_tmp);
    bigint_copy(result, result_tmp);

    /* Free Bigint */
    bigint_delete(&result_tmp);
}

/**
 * @brief Exponentiation using the montgomery ladder method with modular reduction.
 * 
 * Calculates the result of exponentiation using the montgomery ladder method
 * and reduces the result modulus.
 * [Result] = [base]^[exponent] % [modular].
 * Note that montgomery method is constant time.
 * You use this algorithm if you want secure algorithm.
 * 
 * @param result [output] Result of modulo exponentiation.
 * @param base [input] The base value.
 * @param exponent [input] The exponent value.
 * @param modular [input] The modulus value.
 */
void bigint_exponentiation_modular_montgomery_ladder(Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular)
{
    /* base and exponent are not NEGATIVE */
    if(base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case : base or exponent must be positive.\n");
        return;
    }

    /* Allocate Bigint and initailize for algorithm */
    Bigint* left = NULL; // result
    Bigint* right = NULL;
    bigint_set_one(&left); // left = 1
    bigint_copy(&right, base);           // right = x

    /* Iteration count */
    Word bitlen = bigint_get_bit_length(exponent);

    /* Montgomery Ladder */
    while (bitlen--)
    {
        if (bigint_get_bit(exponent, bitlen)) {
            bigint_multiplication_textbook(&left, left, right);  // always multiplication
            //bigint_modular_barrett();
            //bigint_squaring(&right, right);             // always squaring
            //bigint_modular_barrett();
        } 
        else {
            bigint_multiplication_textbook(&right, right, left); // note: result is in right. not left.
            //bigint_modular_barrett();
            //bigint_squaring(&left, left);               // note: result is int left. not right
            //bigint_modular_barrett();
        }
    }

    /* Get result */
    bigint_refine(left);
    bigint_copy(result, left);

    /* Free Bigint */
    bigint_delete(&left);
}