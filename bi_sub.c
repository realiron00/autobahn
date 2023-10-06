#include "autobahn.h"

void SUBC(bi* A, bi* B, bi* Z)
{
    
}

/**************************************************************
*	두 큰 정수의 뺄셈을 해주는 함수(A-B=Z)
*
*   A,B : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   Z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
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
            SUBC(A,B,Z);
            return;
        }
        else {
            SUBC(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    else if(A->sign, B->sign == NEGATIVE) {
        if (bi_cmp(A,B)==1) {//A>B
            SUBC(A,B,Z);
            Z->sign=NEGATIVE;
            return;
        }
        else {
            SUBC(B,A,Z);
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