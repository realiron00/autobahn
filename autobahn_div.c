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
void bi_long_div(bi** q, bi** r, bi* x, bi* y)
{

}