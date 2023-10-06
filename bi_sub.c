#include "autobahn.h"

//x-y=z
/*
: procedure SUB(A, B)
2: if A = 0 then
3: return −B
4: end if
5: if B = 0 then
6: return A
7: end if
8: if A = B then
9: return 0
10: end if
11: if 0 < B ≤ A then
12: return SUBC(A, B)
13: else if 0 < A < B then
14: return −SUBC(B, A)
15: end if
16: if 0 > A ≥ B then
17: return SUBC(|B|, |A|)
18: else if 0 > B > A then
19: return −SUBC(|A|, |B|)
20: end if
21: if A > 0 and B < 0 then
22: return ADD(A, |B|)
23: else . Case: A < 0 and B > 0
24: return −ADD(|A|, B)
25: end if
26: end procedure
*/
void SUB(bi* A, bi* B, bi* Z)
{
	if (bi_is_zero(A)) {
		bi_cpy(&Z, B);
        if(B->sign==POSITIVE) Z->sign=NEGATIVE;
        else Z->sign=POSITIVE;
		return;
	}
	
	if (bi_is_zero(B)) {
		bi_cpy(&Z, A);
        return;
	}
	
	if (bi_cmp(A,B)==0){
		bi_set_zero(Z);
		return;
	}
		
	if (A->sign, B->sign == POSITIVE) {
        if (bi_cmp(A,B)==1) {//A>B
            bi_subc(A,B,Z);
            return;
        }
        else {
            bi_subc(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    else if(A->sign, B->sign == NEGATIVE) {
        if (bi_cmp(A,B)==1) {//A>B
            bi_subc(A,B,Z);
            Z->sign=NEGATIVE;
            return;
        }
        else {
            bi_subc(B,A,Z);
            return;
        }
    }

    else if(A->sign==POSITIVE && B->sign==NEGATIVE) {
        ADD(A,B,Z);
        return;
    }

    else if(A->sign==NEGATIVE && B->sign==POSITIVE) {
        ADD(A,B,Z);
        Z->sign=NEGATIVE;
        return;
    }
	
}