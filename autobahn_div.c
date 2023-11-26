#include "autobahn.h"

/****************************************************************************************
 * bi_div_discriminant
 * x=yq+r(0<=r<y)
 * 
 * input
 * q: quotient
 * r: remainder
 * x: dividend
 * y: divisor
 ****************************************************************************************/
void bi_div_discriminant(bi** q, bi** r, bi* x, bi* y)
{
    //if y <= 0 or x < 0, return INVALID
    if ((bi_is_zero(y) && y->sign == 1)|| x->sign == 1)
    {
        exit(1);
    }

    //if x < y, return q = 0, r = x
    if (bi_cmp(x,y) == -1)
    {
        bi_set_zero(q);
        bi_cpy(r,x);
        return;
    }

    //if y = 1, return q = x, r = 0
    if(bi_is_one(y))
    {
        bi_cpy(q,x);
        bi_set_zero(r);
        return;
    }
}

/****************************************************************************************
 * bi_div_long
 * x=yq+r(0<=r<y)
 * 
 * input
 * q: quotient
 * r: remainder
 * x: dividend
 * y: divisor
 ****************************************************************************************/
void bi_div_long(bi** q, bi** r, bi* x, bi* y)
{
    bi_div_discriminant(q, r, x, y);

    //q_temp : temporary quotient
    //r_temp : temporary remainder
    bi* q_temp = NULL;
    bi* r_temp = NULL;
    bi_new(&q_temp, (x->dmax)-(y->dmax)+1);
    bi_new(&r_temp, y->dmax);
    bi_set_zero(&q_temp);
    bi_set_zero(&r_temp);

    //bi_two : bi storing 2
    bi* bi_two = NULL;
    bi_new(&bi_two, 1);
    bi_two->a[0]=0x02;

    //two_squ : bi storing 2^j
    bi* two_squ=NULL;
    bi_new(&two_squ, (x->dmax)-(y->dmax)+1);
    two_squ->a[0]=0x01;

    //x_bit : bi storing 1 bit of x
    bi* x_bit = NULL;
    bi_new(&x_bit, 1);

    //Divide x into blocks of 32 bits and calculate
    for(int block_idx=x->dmax-1; block_idx>=0; block_idx--)
    {
        //Calculate bitwise
        for(int bit_idx=32-1; bit_idx>=0; bit_idx--)
        {
            //store bit_idx-th bit of x into x_bit
            x_bit->a[0]=(x->a[block_idx] & (1 << bit_idx)) >> bit_idx;

            //remainder = remainder * 2 + x_bit
            bi_textbook_mul(&r_temp,r_temp,bi_two);
            bi_add(&r_temp,r_temp,x_bit);
            if(bi_cmp(r_temp, y) < 0)
            {
                continue;
            }
            //store 2^bit_idx into two_squ
            for(int j=bit_idx+32*block_idx; j>0; j--)
            {
                bi_textbook_mul(&two_squ,two_squ,bi_two);
            }
            //quotient = quotient + 2^bit_idx
            bi_add(&q_temp,q_temp,two_squ);

            //remainder = remainder - divisor
            bi_sub(&r_temp,r_temp,y);

            //initialize two_squ
            bi_set_zero(&two_squ);
            two_squ->a[0]=0x01;
        }
	}

    //refine quotient and remainder to remove leading zeros
    bi_refine(q_temp);
    bi_refine(r_temp);

    //copy quotient and remainder to output variables q and r
    bi_cpy(q,q_temp);
    bi_cpy(r,r_temp);

    //Delete the temporary storage
    bi_delete(&q_temp);
    bi_delete(&r_temp);
    bi_delete(&bi_two);
    bi_delete(&two_squ);
    bi_delete(&x_bit);
    
    return;
}

/****************************************************************************************
 * bi_div_naive
 * x=yq+r(0<=r<y)
 * 
 * input
 * q: quotient
 * r: remainder
 * x: dividend
 * y: divisor
 ****************************************************************************************/
void bi_div_naive(bi** q, bi** r, bi* x, bi* y)
{
    bi_div_discriminant(q, r, x, y);

    //q_temp : temporary quotient
    //r_temp : temporary remainder
    bi* q_temp = NULL;
    bi* r_temp = NULL;
    bi_new(&q_temp, (x->dmax)-(y->dmax)+1);
    bi_new(&r_temp, y->dmax);

    bi_set_zero(&q_temp); //initialize q_temp
    bi_cpy(&r_temp,x); //r_temp = x

    //bi_one : bi storing 1
    bi* bi_one = NULL;
    bi_new(&bi_one, 1);
    bi_one->a[0]=0x01;

    //Naive division
    while(bi_cmp(r_temp,y)>=0) //while remainder >= divisor
    {
        //quotient = quotient + 1
        bi_add(&q_temp,q_temp,bi_one);

        //remainder = remainder - divisor
        bi_sub(&r_temp,r_temp,y);
    }

    //refine quotient and remainder to remove leading zeros
    bi_refine(q_temp);
    bi_refine(r_temp);

    //copy quotient and remainder to output variables q and r
    bi_cpy(q,q_temp);
    bi_cpy(r,r_temp);

    //Delete the temporary storage
    bi_delete(&q_temp);
    bi_delete(&r_temp);
    bi_delete(&bi_one);
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
void word2_long_div(bi_word* q, bi_word x1, bi_word x0, bi_word y)
{
    printf("word2_long_div\n");
    bi_word q_word = 0;

    bi* q_temp = NULL;
    bi_new(&q_temp, 1);
    bi_set_zero(&q_temp);

    bi* r_temp = NULL;
    bi_new(&r_temp, 2);
    r_temp->a[0] = x1;

    bi* temp_231 = NULL;
    bi_new(&temp_231, 1);
    temp_231->a[0] = 0x80000000;

    bi* aj_temp = NULL;//!
    bi_new(&aj_temp, 1);

    bi* y_temp = NULL;//!
    bi_new(&y_temp, 1);
    y_temp->a[0] = y;

    bi_word bit_idx=0;//!

    for(int j=31;j>=0;j--)
    {
        bi_refine(r_temp);
        if(bi_cmp(r_temp, temp_231) >= 0)
        {
            q_word = q_word + (1 << j);

            bi_add(&r_temp, r_temp, r_temp);

            aj_temp->a[0] = ((x0 & (1 << j)) >> j);

            bi_add(&r_temp, r_temp, aj_temp);

            bi_sub(&r_temp, r_temp, y_temp);

            q_temp->a[0] = q_word;
        }
        else
        {
            bi_add(&r_temp, r_temp, r_temp);

            bit_idx = j % BITLEN_OF_WORD;

            aj_temp->a[0] = (x0 >> bit_idx) & 1;

            bi_add(&r_temp, r_temp, aj_temp);

            if(bi_cmp(r_temp, y_temp) >= 0)
            {
                q_word = q_word + (1 << j);
                bi_sub(&r_temp, r_temp, y_temp);
                q_temp->a[0] = q_word;
            }
        }
    }

    *q = q_temp->a[0];

    bi_delete(&q_temp);
    bi_delete(&r_temp);
    bi_delete(&temp_231);
    bi_delete(&aj_temp);
    bi_delete(&y_temp);

    return;

//     bi* q_temp = NULL;
//     bi_new(&q_temp, 1);
//     bi_set_zero(&q_temp);

//     bi* r_temp = NULL;
//     bi_new(&r_temp, 1);
//     bi_set_zero(&r_temp);

//     bi* x_temp = NULL;
//     bi_new(&x_temp, 2);
//     x_temp->a[0] = x0;
//     x_temp->a[1] = x1;

//     bi* y_temp = NULL;
//     bi_new(&y_temp, 1);
//     y_temp->a[0] = y;

//     bi_div_long(&q_temp, &r_temp, x_temp, y_temp);

//     *q = q_temp->a[0];
//     printf("77777. q_temp: %d\n", q_temp->a[0]);
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
void divcc(bi** q, bi** r, bi* x, bi* y)
{
    printf("divcc\n");
    bi* q_temp = NULL;
    bi_new(&q_temp, 1);
    bi_set_zero(&q_temp);

    bi* r_temp = NULL;
    bi_new(&r_temp, y->dmax);
    bi_set_zero(&r_temp);

    //if n = m then
    if(x->dmax == y->dmax)
    {
        //Q ← A_m-1/B_m-1 . Q ∈ [0, W)
        bi_word temp22 = x->a[x->dmax-1] / y->a[y->dmax-1];
        q_temp->a[0] = temp22;
    }

    //if n = m + 1 then
    else
    {
        //if A_m = B_m−1 then
        if(x->a[y->dmax] == y->a[y->dmax-1])
        {
            //Q ← W − 1
            q_temp->a[0] = 0xFFFFFFFF;
        }
        //else . A_m < B_m−1
        else if(x->a[y->dmax] < y->a[y->dmax-1])
        {
            bi_word q_temp2=0;
            //x_temp1 : bi_word storing x->a[y->dmax]
            //x_temp2 : bi_word storing x->a[y->dmax-1]
            bi_word x_temp1 = x->a[y->dmax];
            bi_word x_temp2 = x->a[y->dmax-1];
            //y_temp : bi_word storing y->a[y->dmax-1]
            bi_word y_temp = y->a[y->dmax-1];

            word2_long_div(&q_temp2, x_temp1, x_temp2, y_temp);
            q_temp->a[0] = q_temp2;
        }
    }
    printf("1\n");

    //R ← A − BQ . Q ∈ [0, W)
    bi* temp = NULL;
    bi_new(&temp, y->dmax+1);
    bi_textbook_mul(&temp, q_temp, y);
    bi_sub(&r_temp, x, temp);
    printf("2\n");

    bi* bi_one = NULL;
    bi_set_one(&bi_one);
    printf("3\n");

    // printf("r_temp->sign: %d\n", r_temp->sign);
    // printf("q_temp : ");
    // for(int i=0; i<q_temp->dmax; i++)
    // {
    //     printf("0x%x ", q_temp->a[i]);
    // }
    // printf("\n");
    // printf("r_temp : ");
    // for(int i=0; i<r_temp->dmax; i++)
    // {
    //     printf("0x%x ", r_temp->a[i]);
    // }
    // printf("\n");
    // printf("y : ");
    // for(int i=0; i<y->dmax; i++)
    // {
    //     printf("0x%x ", y->a[i]);
    // }
    // printf("\n");

    //while R < 0 do
    printf(r_temp->sign == NEGATIVE ? "r_temp->sign == NEGATIVE\n" : "r_temp->sign != NEGATIVE\n");
    int jjj=0;
    printf("r_temp : ");
    for(int i=0; i<r_temp->dmax; i++)
    {
        printf("0x%x ", r_temp->a[i]);
    }
    printf("\n");

    printf("q_temp : ");
    for(int i=0; i<q_temp->dmax; i++)
    {
        printf("0x%x ", q_temp->a[i]);
    }
    printf("\n");

    printf("y : ");
    for(int i=0; i<y->dmax; i++)
    {
        printf("0x%x ", y->a[i]);
    }
    printf("\n");
    while(r_temp->sign == NEGATIVE)
    {
        //(Q, R) ← (Q − 1, R + B) . At most 2 computations
        bi_sub(&q_temp, q_temp, bi_one);
        // printf("q_temp : ");
        // for(int i=0; i<q_temp->dmax; i++)
        // {
        //     printf("%d ", q_temp->a[i]);
        // }
        // printf("\n");
        bi_add(&r_temp, r_temp, y);
        // printf("r_temp : ");
        // for(int i=0; i<r_temp->dmax; i++)
        // {
        //     printf("%d ", r_temp->a[i]);
        // }
        // printf("\n");
    }
    jjj=0;
    printf("4\n");

    //copy quotient and remainder to output variables q and r
    bi_cpy(q,q_temp);
    bi_cpy(r,r_temp);
    printf("5\n");

    //Delete the temporary storage
    bi_delete(&q_temp);
    bi_delete(&r_temp);
    bi_delete(&temp);
    bi_delete(&bi_one);

    return;
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
void divc(bi** q, bi** r, bi* x, bi* y)
{
    printf("divc\n");
    //if x < y, return q = 0, r = x
    if (bi_cmp(x,y) == -1)
    {
        bi_set_zero(q);
        bi_cpy(r,x);
        return;
    }
    
    //find k such that 2^k * y->a[y->dmax-1] ∈ [2^31, 2^32) (w=32)
    int k = 0;
    bi_word temp = y->a[y->dmax-1];
    while(1)
    {
        if(temp >= 0x80000000) break;
        
        temp = temp << 1;
        k++;
    }

    //two_squ : bi storing 2^k 
    bi* two_squ=NULL;
    bi_new(&two_squ, 1);
    two_squ->a[0]=0x01;

    //bi_two : bi storing 2
    bi* bi_two = NULL;
    bi_new(&bi_two, 1);
    bi_two->a[0]=0x02;

    //2^k
    for(int j=0; j<k; j++)
    {
        bi_textbook_mul(&two_squ,two_squ, bi_two);
    }

    //A', B' ← 2^k*A, 2^k*B
    bi* x_temp = NULL;
    bi_new(&x_temp, x->dmax+1);
    bi_textbook_mul(&x_temp, x, two_squ);

    bi* y_temp = NULL;
    bi_new(&y_temp, y->dmax+1);
    bi_textbook_mul(&y_temp, y, two_squ);

    //q_temp : Q'
    bi* q_temp = NULL; 
    bi_new(&q_temp, (x_temp->dmax)-(y_temp->dmax)+1);
    bi_set_zero(&q_temp);

    //r_temp : R'
    bi* r_temp = NULL;
    bi_new(&r_temp, y_temp->dmax);
    bi_set_zero(&r_temp);

    bi_refine(x_temp);
    bi_refine(y_temp);

    //Q', R' ← DIVCC(A', B')
    divcc(&q_temp, &r_temp, x_temp, y_temp);

    //R ← 2^−k*R'
    shift_bit_right(&r_temp, k);

    //Q, R ← Q', 2^−k*R'
    bi_cpy(q, q_temp);
    bi_cpy(r, r_temp);

    //Delete the temporary storage
    bi_delete(&two_squ);
    bi_delete(&bi_two);
    bi_delete(&x_temp);
    bi_delete(&y_temp);
    bi_delete(&q_temp);
    bi_delete(&r_temp);

    return;
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
void bi_div_general_long(bi** q, bi** r, bi* x, bi* y)
{
    printf("bi_div_general_long\n");
    //discriminant
    bi_div_discriminant(q, r, x, y);

    //r_temp : temporary remainder
    bi* r_temp = NULL;
    bi_new(&r_temp, y->dmax);
    bi_set_zero(&r_temp); //initialize r_temp

    //q_temp : temporary quotient
    bi* q_temp = NULL;
    bi_new(&q_temp, (x->dmax)-(y->dmax)+1);
    bi_set_zero(&q_temp); //initialize q_temp

    //q_temp2 : temporary quotient[i]
    bi* q_temp2 = NULL;
    bi_new(&q_temp2, 1);
    bi_set_zero(&q_temp2);
//!
    //r_temp2 : temporary remainder[i+1]
    bi* r_temp2 = NULL;
    bi_new(&r_temp2, 2);
    bi_set_zero(&r_temp2);

    //x_temp : temporary x[i]
    bi* x_temp = NULL;
    bi_new(&x_temp, 1);
    bi_set_zero(&x_temp);

    //rwa : temporary remainder[i+1]*W + x[i]
    bi* rwa = NULL;
    bi_new(&rwa, y->dmax+1);

    for(int i=x->dmax-1; i>=0; i--)
    {
        //rwa = r_temp * W + x->a[i]
        r_temp2->a[1] = r_temp->a[0];
        x_temp->a[0] = x->a[i];
        bi_add(&rwa, r_temp2, x_temp);
//!11261601
        //DIVC(rwa, y)
        divc(&q_temp2, &r_temp, rwa, y);

        //Q=Q_0 + Q_1*W + ... + Q_n−1*W^n−1
        q_temp->a[i] = q_temp2->a[0];
    }

    //refine quotient and remainder to remove leading zeros
    bi_refine(q_temp);
    bi_refine(r_temp);

    //copy quotient and remainder to output variables q and r
    bi_cpy(q,q_temp);
    bi_cpy(r,r_temp);

    //Delete the temporary storage
    bi_delete(&r_temp);
    bi_delete(&q_temp);
    bi_delete(&r_temp2);
    bi_delete(&x_temp);
    bi_delete(&rwa);

    return;
}