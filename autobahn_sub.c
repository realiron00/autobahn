#include "autobahn.h"

/**
 * @brief Perform block-wise subtraction using borrow (__sbb) for big integers (A - B - C = D).
 * 
 * This function performs block-wise subtraction using borrow for big integers.
 * Given three big integer values A, B, and a borrow value C, it calculates the
 * difference D = A - B - C. The result D is stored in the 'z' parameter, and the
 * next borrow value is stored in 'b'.
 * 
 * @param z Pointer to a bi_uword where the result of subtraction (D) will be stored.
 * @param b Pointer to a bi_uword representing the borrow value (C).
 * @param x Pointer to a bi_uword representing the first operand (A).
 * @param y Pointer to a bi_uword representing the second operand (B).
 * 
 * @note
 * - This function performs block-wise subtraction with borrow, considering the
 *   values of 'x', 'y', and the current borrow value 'b'.
 * - It calculates D = (A - b) - B - C.
 * - The next borrow value is calculated and stored in 'b'.
 * - All values (D, b, x, y) are treated as unsigned big integers.
 * - The result D is updated in the 'z' parameter.
 */
static void __sbb(bi_uword* z, bi_uword* b, const bi_uword* x, const bi_uword* y)
{
    bi_uword n_b = 0; // Next borrow value (n_b)

    // Calculate (x - b) mod 2^(WORDSIZE*8)
    *z = *x - *b;

    // Check if a borrow occurs if x < b
    n_b = (*x < *b);

    // Check if a borrow occurs if (x - b) < y
    n_b += (*z < *y);

    // Calculate (x - b - y) mod 2^(WORDSIZE*8)
    *z -= *y;

    // Store the next borrow value
    *b = n_b;
}


/**
 * @brief Perform unsigned subtraction (bi_usub) of two large positive big integers (A - B = Z).
 * 
 * This function performs unsigned subtraction of two large positive big integers, 'x'
 * and 'y', and stores the result in 'z'. The subtraction is done block by block,
 * and any borrow values are handled. The result 'z' is a new big integer, and
 * any leading zeros are removed to refine the result.
 * 
 * @param z Pointer to a bi structure where the result of the unsigned subtraction will be stored.
 * @param x Pointer to a bi structure representing the minuend 'A'.
 * @param y Pointer to a bi structure representing the subtrahend 'B'.
 * 
 * @note
 * - If the dmax of 'y' is different from that of 'x', the function reallocates
 *   memory for 'y' to match the dmax of 'x'.
 * - The function performs block-wise subtraction, considering the values of 'x'
 *   and 'y' as unsigned big integers.
 * - Any borrow values during subtraction are handled internally.
 * - The result 'z' is refined to remove leading zeros.
 */
static void bi_usub(bi** z, const bi* x, bi* y)
{
    // If the dmax of 'y' is different from that of 'x', reallocate 'y' to match 'x'
    if (y->dmax != x->dmax)
        (y->a) = (bi_uword*)realloc(y->a, (x->dmax) * sizeof(bi_uword));

    // Initialize the borrow value 'b' to zero
    bi_uword b = 0, d=0;

    // Create a temporary big integer 'sub' to store the result of subtraction
    bi* sub = NULL;
    bi_new(&sub, x->dmax);

    // Perform block-wise subtraction, considering borrow values
    for (int j=0;j<x->dmax;j++) {
        //subtraction of each block
        __sbb(&d, &b, &(x->a[j]), &(y->a[j]));
        sub->a[j]=d; //store result of subtraction to sub
        d=0; //initialize d
    }

    // Refine 'y' to remove leading zeros
    bi_refine(y);

    // Refine 'sub' to remove leading zeros
    bi_refine(sub);

    // Copy 'sub' to 'z' to store the result
    bi_cpy(z, sub);

    // Delete the temporary storage for 'sub'
    bi_delete(&sub);
}


/**
 * @brief Perform binary subtraction (bi_sub) of two big integers.
 * 
 * This function subtracts two big integers 'x' and 'y' and stores the result
 * in 'z'. The subtraction considers the signs of the operands and performs
 * the appropriate subtraction. The result 'z' is stored as a new big integer,
 * and any leading zeros in 'z' are removed to refine the result.
 * 
 * @param z Pointer to a bi structure where the result of the subtraction will be stored.
 * @param x Pointer to a bi structure representing the first operand 'x'.
 * @param y Pointer to a bi structure representing the second operand 'y'.
 * 
 * @note
 * - The function checks the signs of 'x' and 'y' and performs addition or
 *   subtraction accordingly.
 * - If 'x' and 'y' have different dmax values, 'y' may be reallocated to match
 *   the dmax of 'x'.
 * - Special cases are handled:
 *   - If 'x' is equal to zero, 'y' is copied to 'z'.
 *   - If 'y' is equal to zero, 'x' is copied to 'z'.
 *   - If 'x' is equal to 'y', 'z' will be set to zero.
 * - The result 'z' is refined to remove leading zeros.
 */
void bi_sub(bi** z, bi* x, bi* y)
{
    // If x = 0, set z = -y or y
    if (bi_is_zero(x)) {
        bi_cpy(z, y);
        if (y->sign == POSITIVE)
            (*z)->sign = NEGATIVE;
        else
            (*z)->sign = POSITIVE;
        return;
    }

    // If y = 0, set z = x
    if (bi_is_zero(y)) {
        bi_cpy(z, x);
        return;
    }

    // If x = y, set z = 0
    if (bi_cmp(x, y) == 0) {
        bi_set_zero(z);
        return;
    }

    // Handle cases where both x and y are positive
    if (x->sign == POSITIVE && y->sign == POSITIVE) {
        if (bi_cmp(x, y) == 1) {
            // z = x - y
            bi_usub(z, x, y);
            return;
        } else {
            // z = -(y - x)
            bi_usub(z, y, x);
            (*z)->sign = NEGATIVE;
            return;
        }
    }

    // Handle cases where both x and y are negative
    else if (x->sign == NEGATIVE && y->sign == NEGATIVE) {
        if (bi_cmp(x, y) == 1) {
            // z = |y| - |x|
            bi_usub(z, y, x);
            return;
        } else {
            // z = -(|x| - |y|)
            bi_usub(z, x, y);
            (*z)->sign = NEGATIVE;
            return;
        }
    }

    // Handle cases where one operand is positive and the other is negative
    else if (x->sign == POSITIVE && y->sign == NEGATIVE) {
        // z = x + |y|
        bi_add(z, x, y);
        return;
    } else if (x->sign == NEGATIVE && y->sign == POSITIVE) {
        // z = -(|x| + y)
        bi_add(z, x, y);
        (*z)->sign = NEGATIVE;
        return;
    }
}
