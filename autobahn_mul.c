#include "autobahn.h"

void ShiftLeft(bi** z, int shiftAmount) {
    // If shiftAmount is less than or equal to 0 or the bi structure is empty, do nothing.
    if (shiftAmount <= 0 || !z || !(*z)) {
        return;
    }

    // Create a temporary bignum (temp) with an increased block count to accommodate the left shift.
    bi* temp = NULL;

    bi_new(&temp, (*z)->dmax + shiftAmount);

    // Copy the left-shifted values from z to the temp structure.
    for (int i = 0; i < (*z)->dmax; i++) {

        int sourceIndex = i;
        int destinationIndex = i + shiftAmount;

        if (destinationIndex < temp->dmax) {

            temp->a[destinationIndex] = (*z)->a[sourceIndex];
        }
    }

    // Copy the contents of temp back to z.
    bi_cpy(z, temp);

    // Clean up memory by deleting the temp structure.
    bi_delete(&temp);
}

void ShiftRight(bi** z, int shiftAmount) {
    // If shiftAmount is less than or equal to 0 or the bi structure is empty, do nothing.
    if (shiftAmount <= 0 || !z || !(*z)) {
        return;
    }

    // Create a temporary bignum (temp) with the same block count as z.
    bi* temp = NULL;
    bi_new(&temp, (*z)->dmax);

    // Copy the right-shifted values from z to the temp structure.
    for (int i = 0; i < (*z)->dmax; i++) {
        int sourceIndex = i;
        int destinationIndex = i - shiftAmount;  // Shift to the right.

        if (destinationIndex >= 0) {
            temp->a[destinationIndex] = (*z)->a[sourceIndex];
        }
    }

    // Copy the contents of temp back to z.
    bi_cpy(z, temp);

    // Clean up memory by deleting the temp structure.
    bi_delete(&temp);
}


void arr_mod(bi** z, int k) {

    // Create a temporary bignum (temp) with the same block count as z.
    bi* temp = NULL;
    bi_new(&temp, (*z)->dmax - k);

    for (int i = 0; i < (*z)->dmax - k; i++) {

        temp->a[i] = (*z)->a[i];
    }

    // Copy the contents of temp back to z.
    bi_cpy(z, temp);

    // Clean up memory by deleting the temp structure.
    bi_delete(&temp);
}




// Single_Word_Multiplication
void __mul(bi** z, bi_uword* x, bi_uword* y) {

    int bit_size = sizeof(bi_uword) * 8; // Bit size of a bi_uword
    bi* result = NULL; // Declare a variable to store the result

    // Initialize the result as a 2-word bignum
    bi_new(&result, 2);
    bi_uword A1 = 0;
    bi_uword A0 = 0;
    bi_uword B1 = 0;
    bi_uword B0 = 0;


    // Split the input numbers into their upper and lower halves
    A1 = (*x) >> (bit_size / 2); // Upper half of x
    A0 = (A1 << (bit_size / 2)) ^ (*x); // Lower half of x
    B1 = (*y) >> (bit_size / 2); // Upper half of y
    B0 = (B1 << (bit_size / 2)) ^ (*y); // Lower half of y

    /*
    Multiplication of two 2-word numbers:
                  A1   A0
        x         B1   B0
        ------------------
                | A0   B0 |  T2
            | A1 B0 |     T0
            | A0 B1 |     T1
        | A1  B1 |    T3
    */
    bi_uword T0 = 0;
    bi_uword T1 = 0;
    bi_uword T2 = 0;
    bi_uword T3 = 0;
    bi_uword Temp0 = 0;
    bi_uword Right = 0;
    bi_uword Left = 0;

    // Perform multiplication
    T0 = A1 * B0;
    T1 = A0 * B1;
    T2 = A0 * B0;
    T3 = A1 * B1;

    Temp0 = T0 + T1; // Temp0 = T0 + T1

    Right = (Temp0 << (bit_size / 2)) + T2; // Lower half of the result
    Left = T3 + (Temp0 >> (bit_size / 2)) + ((Temp0 < T0) << (bit_size / 2)) + (Right < T2);  // Upper half of the result - T3 + Temp0 + 'if Temp0 carries, add 1' + 'if Right carries, add 1'

    result->a[0] = Right; // Store the lower half in the result
    result->a[1] = Left;  // Store the upper half in the result

    // Refine the result to remove leading zeros
    bi_refine(result);

    // Copy the result to the output variable z
    bi_cpy(z, result);

    // Free the memory used by the result
    bi_delete(&result);
}

// Improved multiplication function
void bi_mul_improved(bi** z, bi* x, bi* y) {

    int n, m;

    n = x->dmax; // Number of blocks in x
    m = y->dmax; // Number of blocks in y
    bi* mul = NULL; // Declare a variable to store the result (mul)

    // Initialize mul with the appropriate number of blocks
    bi_new(&mul, n + m);

    for (int i = 0; i < n; i++) {

        for (int k = 0; k < m; k++) {

            bi* Temp = NULL;

            bi_new(&Temp, 1);

            // Multiply the i-th block of y and the k-th block of x and shift the result accordingly
            __mul(&Temp, &(x->a[i]), &(y->a[k]));

            // Shift the result left by i + k blocks
            ShiftLeft(&Temp, i + k);

            // Add the result to the running total
            bi_add(&mul, Temp, mul);
            bi_delete(&Temp);
        }

    }

    // Refine the final result to remove leading zeros
    bi_refine(mul);

    // Copy the final result to the output variable z
    bi_cpy(z, mul);

    // Free the memory used by the result
    bi_delete(&mul);
}


//   karatsuba 
void bi_mul_karatsuba(bi** z, bi* x, bi* y) {

    bi* A0 = NULL;
    bi* A1 = NULL;
    bi* B0 = NULL;
    bi* B1 = NULL;
    bi* T0 = NULL;
    bi* T1 = NULL;
    bi* R = NULL;
    bi* S0 = NULL;
    bi* S1 = NULL;
    bi* S = NULL;
    bi* C = NULL;

    bi_new(&A0, 1);
    bi_cpy(&A0, x);
    bi_new(&A1, 1);
    bi_cpy(&A1, x);
    bi_new(&B0, 1);
    bi_cpy(&B0, y);
    bi_new(&B1, 1);
    bi_cpy(&B1, y);
    bi_new(&T0, 1);
    bi_new(&T1, 1);
    bi_new(&R, 1);
    bi_new(&S0, 1);
    bi_new(&S1, 1);
    bi_new(&S, 1);
    bi_new(&C, 1);



    int flag = 10; // 임시 플래그
    int w = sizeof(bi_word) * 8; // 비트 크기
    int min = (((x->dmax < y->dmax) ? (x->dmax) : (y->dmax)));

    if (flag >= min) {
        bi_mul_improved(z, x, y);

        return; // flag 만족 여부
    }

    else {
        int l = (min + 1) >> 1;
        int lw = l * w;
        // Shift_연산
        ShiftRight(&A1, lw);
        arr_mod(&A0, lw);
        ShiftRight(&B1, lw);
        arr_mod(&B0, lw);


        bi_mul_karatsuba(&T1, A1, B1);
        bi_mul_karatsuba(&T0, A0, B0);

        ShiftLeft(&T1, 2 * lw);
        bi_add(&R, T1, T0);
        ShiftRight(&T1, 2 * lw);
        bi_sub(&S1, A0, A1);
        bi_sub(&S0, B0, B1);

        // 구현 해야함 부호 구분 - 질문
        int S1_sign = S1->sign;
        int S0_sign = S0->sign;

        bi_mul_karatsuba(&S, S1, S0);
        S->sign = S1_sign ^ S0_sign;
        //////////////////////////

        bi_add(&S, S, T1);
        bi_add(&S, S, T0);

        ShiftLeft(&S, lw);

        bi_add(&C, C, S);

        // Refine the final result to remove leading zeros
        bi_refine(C);

        // Copy the final result to the output variable z
        bi_cpy(z, C);

    }




    bi_delete(&A0);
    bi_delete(&A1);
    bi_delete(&B0);
    bi_delete(&B1);
    bi_delete(&T0);
    bi_delete(&T1);
    bi_delete(&R);
    bi_delete(&S1);
    bi_delete(&S0);
    bi_delete(&S);
    bi_delete(&C);
};





void bi_mul(bi** z, bi* x, bi* y) {

    if (bi_is_zero(x) || bi_is_zero(y)) {

        (*z)->a[0] = 0;
        return;
    }

    if (bi_is_one(x)) {
        bi_cpy(z, y);
        return;
    }

    if (bi_is_one(y)) {
        bi_cpy(z, x);
        return;
    }

    // 부호 파트 
    (*z)->sign = x->sign ^ y->sign;
    //////////////////////////
    bi_mul_improved(z, x, y);

}