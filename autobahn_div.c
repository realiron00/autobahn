#include "autobahn.h"

/*
Input: A = sum(a_j*2^j)(j=0 to n-1)
        ,B ∈ Z (a_j ∈ {0, 1}, A ≥ B > 0)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B).
1: procedure DIVlong-b(A, B)
2: (Q, R) ← (0, 0)
3: for j = n − 1 downto 0 do
4: R ← 2R + aj . R ← (R  1) ⊕ aj
5: if R ≥ B then
6: (Q, R) ← (Q + 2j
, R − B) . Q ← Q ⊕ (1  j)
7: end if
8: end for
9: return (Q, R)
*/
/*
x=yq+r(0<=r<y)
*/

/*
A, B ∈ Z≥0
Output: INVALID or (Q, R) such that A = BQ + R, (0 ≤ R < B).
1: procedure DIVnaive(A, B)
2: if B ≤ 0 then
3: return INVALID
4: end if
5: if A < B then
6: return (0, A)
7: end if
8: if B = 1 then
9: return (A, 0)
10: end if
11: (Q, R) ← (0, A)
12: while R ≥ B do
13: (Q, R) ← (Q + 1, R − B)
14: end while
15: return (Q, R)
16: end procedure

*/
void bi_long_div(bi** q, bi** r, bi* x, bi* y)
{
        //if y<=0, return INVALID
        if (bi_is_zero(y) && y->sign == 1){
                exit(1);
        }

        //if x<y, return q=0, r=x
        if (bi_cmp(x,y)==-1){
                bi_set_zero(q);
                bi_cpy(r,x);
                return;
        }

        //if y=1, return q=x, r=0
        if(bi_is_one(y)){
                bi_cpy(q,x);
                bi_set_zero(r);
                return;
        }
}