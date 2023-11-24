#include "autobahn.h"

static bi_word bi_cmpABS(const bi* x, const bi* y)
{
    bi_word x_dmax = x->dmax;
    bi_word y_dmax = y->dmax;

    //Return 1 if x's dmax is bigger than y's dmax
    if (x_dmax > y_dmax)
        return 1;

    //Return -1 if y's dmax is bigger than x's dmax
    else if (x_dmax < y_dmax)
        return -1;

    //if x_dmax == y_dmax, compare each digit
    for (int i = x_dmax - 1; i >= 0; i--) {
        //Return 1 if the value of the digit in x is bigger than the value of the digit in y
        if (x->a[i] > y->a[i])
            return 1;

        //Return -1 if the value of the digit in y is bigger than the value of the digit in x
        else if (x->a[i] < y->a[i])
            return -1;
    }
    //if all the digits are the same, return 0
    return 0;
}

//shift_left
void shift_left(bi** z, int shiftAmount) {

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
    bi_refine(temp);
    // Copy the contents of temp back to z.
    bi_cpy(z, temp);

    // Clean up memory by deleting the temp structure.
    bi_delete(&temp);
}

// shift_bit_left
void shift_bit_left(bi** z, int shiftAmount) {
    if (shiftAmount <= 0 || !z || !(*z)) {
        return;
    }

    // 비트 단위로 왼쪽으로 이동
    int carry = 0;
    for (int i = 0; i < (*z)->dmax; i++) {
        int newValue = ((*z)->a[i] << shiftAmount) | carry;
        carry = ((*z)->a[i] >> (sizeof(bi_word) * 8 - shiftAmount));
        (*z)->a[i] = newValue;
    }

    bi_refine(*z);
}

// shift_right
void shift_right(bi** z, int shiftAmount) {
    // If shiftAmount is less than or equal to 0 or the bi structure is empty, do nothing.
    if (shiftAmount <= 0 || !z || !(*z)) {
        return;
    }

    // Create a temporary bignum (temp) with the same block count as z.
    bi* temp = NULL;
    bi_new(&temp, (*z)->dmax);
	
	if (shiftAmount >= (*z)->dmax) {
        bi_set_zero(z);
        return;
    }

    // Copy the right-shifted values from z to the temp structure.
    for (int i = 0; i < (*z)->dmax; i++) {
        int sourceIndex = i;
        int destinationIndex = i - shiftAmount;  // Shift to the right.

        if (destinationIndex >= 0) {
            temp->a[destinationIndex] = (*z)->a[sourceIndex];
        }
    }
    bi_refine(temp);
    // Copy the contents of temp back to z.
    bi_cpy(z, temp);

    // Clean up memory by deleting the temp structure.
    bi_delete(&temp);
}

// 
void shift_bit_right(bi** z, int shiftAmount) {
    if (shiftAmount <= 0 || !z || !(*z)) {
        return;
    }

    // 비트 단위로 오른쪽으로 이동
    int carry = 0;
    for (int i = (*z)->dmax - 1; i >= 0; i--) {
        int newValue = ((*z)->a[i] >> shiftAmount) | carry;
        carry = ((*z)->a[i] << (sizeof(bi_word) * 8 - shiftAmount));
        (*z)->a[i] = newValue;
    }

    // Clean up memory by deleting the temp structure.
    bi_refine(*z);
}


// arr_mod
void arr_mod(bi** z, int k) {
    if (k >= (*z)->dmax) {
        // k가 dmax 이상이면 아무 작업도 하지 않고 반환합니다.
        return;
    }

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

// single_word_multiplication
void single_word_mul(bi** z, bi_uword* x, bi_uword* y) {

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
void bi_textbook_mul(bi** z, bi* temp_x, bi* temp_y) {

    bi* x = NULL;
    bi* y = NULL;
    bi_cpy(&x, temp_x);
    bi_cpy(&y, temp_y);


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
            single_word_mul(&Temp, &(x->a[i]), &(y->a[k]));

            // Shift the result left by i + k blocks
            shift_left(&Temp, i + k);
            bi_refine(mul);
            // Add the result to the running total
            bi_add(&mul, Temp, mul);
            bi_delete(&Temp);
        }

    }

    // Refine the final result to remove leading zeros
    bi_refine(mul);

    // Copy the final result to the output variable z
    bi_cpy(z, mul);
    // 부호 처리
    (*z)->sign = (x->sign == y->sign) ? 0 : 1;
    // Free the memory used by the result
    bi_delete(&mul); bi_delete(&x); bi_delete(&y);
}

//   karatsuba 
void bi_karatsuba_mul(bi** z, bi* temp_x, bi* temp_y) {

    bi* x = NULL;
    bi* y = NULL;
	
    bi_cpy(&x, temp_x);
    bi_cpy(&y, temp_y);
	


    int flag = 1; // 임시 플래그
    int w = sizeof(bi_word) * 8; // 비트 크기
    int min = (((x->dmax < y->dmax) ? (x->dmax) : (y->dmax)));
	int max = (((x->dmax > y->dmax) ? (x->dmax) : (y->dmax)));
	if (x->dmax == 1 || y->dmax) {
		bi_textbook_mul(z, x, y);
		bi_delete(&x); bi_delete(&y);
		return; // flag 만족 여부
}
	
    if (flag >= min) {
        bi_textbook_mul(z, x, y);
		bi_delete(&x); bi_delete(&y);
        return; // flag 만족 여부
    }


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

    int l = (max + 1) >> 1;
    int lw = l * w;

    // Shift_연산
    shift_right(&A1, lw);
    arr_mod(&A0, lw);
    shift_right(&B1, lw);
    arr_mod(&B0, lw);


    bi_karatsuba_mul(&T1, A1, B1);
    bi_karatsuba_mul(&T0, A0, B0);

    shift_left(&T1, 2 * lw);
    bi_add(&R, T1, T0);
    shift_right(&T1, 2 * lw);
    bi_sub(&S1, A0, A1);
    bi_sub(&S0, B0, B1);

    // 구현 해야함 부호 구분 - 질문
    int S1_sign = S1->sign;
    int S0_sign = S0->sign;

    bi_karatsuba_mul(&S, S1, S0);
    S->sign = S1_sign ^ S0_sign;
    //////////////////////////

    bi_add(&S, S, T1);
    bi_add(&S, S, T0);

    shift_left(&S, lw);

    bi_add(&C, C, S);

    // Refine the final result to remove leading zeros
    bi_refine(C);

    // Copy the final result to the output variable z
    bi_cpy(z, C);
    // 부호 처리
    (*z)->sign = (x->sign == y->sign) ? 0 : 1;
	bi_delete(&x); bi_delete(&y);
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

// single_word_squ
void bi_single_word_squ(bi** z, bi_uword* x) {

    int bit_size = sizeof(bi_uword) * 8; // Bit size of a bi_uword

    bi* C = NULL; bi* T = NULL;

    // Initialize the result as a 1-word bignum
    bi_new(&C, 1);
    bi_new(&T, 1);

    bi_uword A1 = 0; bi_uword A0 = 0;
    bi_uword C1 = 0; bi_uword C0 = 0;

    // A1, A0 ← A[w:w / 2], A[w / 2:0]
    A1 = (*x) >> (bit_size / 2); // Upper half of x
    A0 = (A1 << (bit_size / 2)) ^ (*x); // Lower half of x

    // C1, C0 ← A1^2,A0^2
    C1 = A1 * A1;
    C0 = A0 * A0;

    // C ← (C1 << w) + C0 
    C1 = (C1 << bit_size);
    C->a[1] = C1; C->a[0] = C0;

    // T ← A0 A1  { T ∈ [0, W) }
    T->a[0] = A0 * A1;

    // T ← T <<(w/2  +1)
    shift_left(&T, 1 + (bit_size / 2));

    // C ← C + T
    bi_add(&C, C, T);

    bi_refine(C);

    bi_cpy(z, C);

    bi_delete(&C); bi_delete(&T);

}

//textbook_squ
void bi_textbook_squ(bi** z, bi* x) {

    bi* C = NULL; bi* C1 = NULL; bi* C2 = NULL;
    bi* T1 = NULL; bi* T2 = NULL;

    bi_new(&C, 1); bi_new(&C1, 1); bi_new(&C2, 1);
    bi_new(&T1, 2); bi_new(&T2, 2);

    // C1, C2 ← 0
    bi_set_zero(&C1); bi_set_zero(&C2);

    // for j=0 to t-1 do
    for (int j = 0; j < x->dmax; j++) {

        bi_single_word_squ(&T1, &(x->a[j])); //T1 ← Aj^2
        shift_left(&T1, 2 * j);//T1 ← T1 << 2jw
        bi_add(&C1, T1, C1); // C1 ← T1 + C1 
        for (int i = j + 1; i < x->dmax; i++) {
            single_word_mul(&T2, &(x->a[j]),&( x->a[i])); // T2 ← AjAi
            shift_left(&T2, i + j); // T2 ← T2 << (i + j)w
            bi_add(&C2, C2, T2); // C2 ← ADD(C2, T2)
        }
    }

    shift_bit_left(&C2, 1); //  C2 ← C2 << 1

    bi_add(&C, C1, C2); // C ← ADD(C1, C2)

    bi_refine(C);

    bi_cpy(z, C);

    bi_delete(&C); bi_delete(&C1); bi_delete(&C2);
    bi_delete(&T1); bi_delete(&T2);

}

void bi_karatsuba_squ(bi** z, bi* x) {

    int flag = 10; int l = 0;
    bi* A1 = NULL; bi* A0 = NULL; 
    bi* T1 = NULL; bi* T0 = NULL;
    bi* R = NULL; bi* S = NULL;
    bi* result = NULL;
    
    bi_new(&A1, 1); bi_new(&A0, 1); 
    bi_new(&T1, 1); bi_new(&T0, 1);
    bi_new(&R, 1); bi_new(&S, 1);
    bi_new(&result, 1);
    if (flag >= x->dmax) {
        bi_textbook_squ(z, x);
        return;
    }

    l = (x->dmax + 1) >> 1; // l ← (wordlen(A) + 1) >> 1

    bi_cpy(&A1, x); bi_cpy(&A0, x);
    
    // A1, A0 ← |A|  lw, |A| mod 2lw 
    shift_right(&A1, l); A1->sign = POSITIVE; 
    arr_mod(&A0,l); A0->sign = POSITIVE;

    // T1, T0 ← SQUCKaratsuba(A1), SQUCKaratsuba(A0) 
    bi_karatsuba_squ(&T1, A1); bi_karatsuba_squ(&T0, A0);

    // R ← (T1 << 2lw) + T0 
    shift_left(&T1, 2 * l);
    bi_add(&R, T1, T0);

    // S ← MULCKaratsuba(A1, A0) 
    bi_karatsuba_mul(&S,A1,A0);

    // S ← S << (lw + 1)
    shift_left(&S, l + 1);
    
    // C ← ADDC(R, S)
    bi_add(&result,R,S);
    bi_refine(result);
    bi_cpy(z,result);

    bi_delete(&A1); bi_delete(&A0);
    bi_delete(&T1); bi_delete(&T0);
    bi_delete(&R); bi_delete(&S);
    bi_delete(&result);

}
/*
// 만들고 있는 함수
// 사전계산
void bi_using_barrett(bi** T, bi* N, int* n) {

    int W = 0;

    W = sizeof(bi_word) * 8; //

    //임시값 랜덤값이 입력되어야 한다.
    bi* bi_W_temp = NULL;
    bi* bi_Q_temp = NULL;
    bi_new(&bi_W_temp, 1);
    bi_new(&bi_Q_temp, 1);
    bi_set_one(&bi_W_temp);

    shift_left(&bi_W_temp, 2*(*n));


    bi_div_long(&bi_Q_temp,&bi_W_temp, bi_W_temp,N); // * 나누기로 수정

    bi_refine(bi_W_temp);
    bi_show_hex(bi_W_temp);
    // bi_set_by_str(&bi_W_temp, "14270517267a00e5b84de", POSITIVE, 16);
    bi_cpy(T, bi_W_temp);
    bi_delete(&bi_W_temp);
    bi_delete(&bi_Q_temp);

}

void bi_mul_barrett(bi** z, bi* temp_A, bi* temp_N, int n) {

    // Admax의 크기가  N 2배이하인지 확인
    if (temp_A->dmax < (temp_N->dmax) * 2)
    {
        printf("NO Using barrett");
        return;
    }
    bi* T = NULL;
    bi* Q_hat = NULL;
    bi* N = NULL;
    bi* A = NULL;
    bi* R = NULL;



    bi_cpy(&N, temp_N);
    bi_cpy(&A, temp_A);


    bi_new(&T, 1);
    bi_new(&Q_hat, 1);
    bi_new(&R, 1);



    bi_using_barrett(&T, N, &n);


    bi_cpy(&Q_hat, A);

    // Q_hat = (( A >> (w *(n -1) )) * T ) >> (w *( n +1) )
    shift_right(&Q_hat, (n - 1));

    bi_textbook_mul(&Q_hat, T, Q_hat);
    shift_right(&Q_hat, (n + 1));

    bi_textbook_mul(&Q_hat, Q_hat, N); // N* Q_hat
    bi_sub(&R, A, Q_hat); // A - N* Q_hat


    while (bi_cmpABS(R, N) != -1) {  // static bi_word bi_cmpABS(const bi* x, const bi* y) 헤더 추가

        bi_sub(&R, R, N); // temp_x, tempy 로 sub 수정

    }

    bi_refine(R);
    bi_cpy(z, R);

    bi_delete(&T);
    bi_delete(&Q_hat);
    bi_delete(&R);



}
*/




void bi_mul(bi** z, bi* x, bi* y) {
    if (bi_is_zero(x) || bi_is_zero(y)) {
        bi_set_zero(z);
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



    // 메모리 할당을 bi_karatsuba_mul 함수 내에서 처리
    bi_karatsuba_mul(z, x, y);
}