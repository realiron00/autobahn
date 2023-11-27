#include "autobahn.h"

/**
 * @brief Pre-computation for Barrett reduction.
 * 
 * Calculates the pre-computed value for Barrett reduction using the modulus [modular].
 * Let W is word and n is number of digits of [modular].
 * Then the [barret_pre_computed] is W^(2n) / N.
 * 
 * @param barrett_pre_computed [output] Result of pre-computation for Barrett reduction.
 * @param modular [input] The modulus value.
 */
void bigint_reduction_barrett_pre_computed(Bigint** barrett_pre_computed, const Bigint* modular) 
{
    /* Allocate Bigint */
    Bigint* remainder = NULL;
    Bigint* quotient = NULL; // This will be pre_computed value in barrett reduction.
    Bigint* dividend = NULL; // W^(2n)
    bigint_new(&remainder, 1);
    bigint_new(&quotient, 1);
    bigint_new(&dividend, 1);

    /* Initialize dividend */
    bigint_set_one(&dividend);
    bigint_expand(&dividend, dividend, 2 * modular->digit_num); // W^(2n)

    /* compute divide */
    bigint_division_binary_long(&quotient, &remainder, dividend, modular); // W^(2n) / N
    
    /* Get result */
    bigint_refine(quotient);
    bigint_copy(barrett_pre_computed, quotient);

    /* Free Bigint */
    bigint_delete(&remainder);
    bigint_delete(&quotient);
    bigint_delete(&dividend);
}

/**
 * @brief Barrett reduction.
 * 
 * Performs Barrett reduction of the given large integer [bigint] using the modulus [modular] 
 * and pre-computed values [pre_computed].
 * 
 * Let A is [bigint], N is [modular] and T is pre_cumputed value
 * (see comment of barret_reduction_pre_computed function).
 * Then the result of reduction R = A mod N.
 * 
 * @param result [output] Result of Barrett reduction.
 * @param bigint [input] The large integer to be reduced.
 * @param modular [input] The modulus value.
 * @param pre_computed [input] Pre-computed values for Barrett reduction.
 */
void bigint_reduction_barrett(Bigint** result, const Bigint* bigint, const Bigint* modular, const Bigint* pre_computed) 
{ 
    /* Check invalid parameter : A is in W^2n */
    if (bigint->digit_num > modular->digit_num * 2) {
        printf("Do not barrett\n");
        return;
    }

    /* Allocate Bigint */
    Bigint* quotient = NULL;
    Bigint* remainder = NULL; // This will be result of barrett reduction.
    bigint_new(&quotient, 1);
    bigint_new(&remainder, 1);

    /* Barrett reduction: Compute quotient: quotient is Q, Q+1 or Q+2 */
    bigint_compress(&quotient, bigint, modular->digit_num - 1);   // Q <- A >> W^(n-1)
    bigint_multiplication_textbook(&quotient, quotient, pre_computed);     // Q <- (A >> W^(n-1)) * T
    bigint_compress(&quotient, quotient, modular->digit_num + 1); // Q <- ((A >> W^(n-1)) * T) >> W^(n+1)

    /* Barrett reduction: Compute remainder: remainder is R, R+N or R+2N */
    bigint_multiplication_textbook(&remainder, quotient, modular); // R <- Q * N
    bigint_subtraction(&remainder, bigint, remainder);    // R <- A - Q * N

    /* Barrett reduction: Find correct result */
    while (bigint_compare(remainder, modular) != LEFT_IS_SMALL)
        bigint_subtraction(&remainder, remainder, modular); // make R if remainder is R + N or R + 2N

    /* Get result */
    bigint_refine(remainder);
    bigint_copy(result, remainder);

    /* Free Bigint */
    bigint_delete(&quotient);
    bigint_delete(&remainder);
}