#include <stdio.h>

int main()
{
    //k=2
    //a=0x14
    //b=2^(-k)*a=0x05
    int j=7;
    int a=0;
    a = 1 << j;
    printf("a: %d\n", a);
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
DIVC(a, b)
input: A = A_0 + A_1*W^1 + ... + A_m*W^m, B = B_0 + B_1*W^1 + ... + B_m-1*W^m-1 (A_j, B_j ∈ [0, W), 0 ≤ A < BW)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B, Q ∈ [0, W)).
1: procedure DIVC(A, B)
2: if A < B then
3:  return (0, A)
4: end if
5: Compute k ∈ Z≥0 such that 2^k*B_(m−1) ∈ [2^31, 2^32)
6: A', B' ← 2^k*A, 2^k*B
7: Q', R' ← DIVCC(A', B')

8: Q, R ← Q', 2^−k*R'
9: return (Q, R)
10: end procedure
*/

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