#include "autobahn.h"

/*
Input: A = sum(a_j*2^j)(j=0 to n-1)
        ,B ∈ Z (a_j ∈ {0, 1}, A ≥ B > 0)
Output: (Q, R) such that A = BQ + R (0 ≤ R < B).
1: procedure DIVlong-b(A, B)
2: (Q, R) ← (0, 0)
3: for j = n − 1 downto 0 do
4: R ← 2R + aj . R ← (R << 1) ⊕ aj
5: if R ≥ B then
6: (Q, R) ← (Q + 2j
, R − B) . Q ← Q ⊕ (1 << j)
7: end if
8: end for
9: return (Q, R)
*/
/*
x=yq+r(0<=r<y)
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

        bi_set_zero(q);
        bi_set_zero(r);
}