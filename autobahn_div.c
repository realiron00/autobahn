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
            bi_mul_improved(&r_temp,r_temp,bi_two);
            bi_add(&r_temp,r_temp,x_bit);
            if(bi_cmp(r_temp, y) < 0)
            {
                continue;
            }
            //store 2^bit_idx into two_squ
            for(int j=bit_idx+32*block_idx; j>0; j--)
            {
                bi_mul_improved(&two_squ,two_squ,bi_two);
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

//bi_div_general_long: Multi-Precision Long Division
//DIVC : 2-word long division
void bi_div_general_long(bi** q, bi** r, bi* x, bi* y)
{
    bi_div_discriminant(q, r, x, y);

    
}