#include "autobahn.h"
/*
1: procedure SUBC(A, B) . A ≥ B > 0
2: Bj ← 0 for j = m, m + 1, . . . , n − 1
3: b ← 0
4: for j = 0 to n − 1 do
5: b, Cj ← SUBAbB(Aj , b, Bj )
6: end for
7: l ← min{j : Cn−1 = Cn−2 = · · · = Cj = 0}
8: return Pl−1
j=0 CjWj
9: end procedure
*/
void SUBC(bi* A, bi* B, bi* Z)
{
    if (B->dmax != A->dmax) {// 크기가 다르면 할당
		(B->a) = (bi_word*)realloc(B->a, (A->dmax) * sizeof(bi_word)); // B의 dmax 할당
	}
	//test.......
}

/**************************************************************
*	두 큰 정수의 뺄셈을 해주는 함수(A-B=Z)
*
*   A,B : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   Z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void SUB(bi* A, bi* B, bi* Z)
{
    //A=0 -> 0-B=Z
	if (bi_is_zero(A)) {
        //Z=-B
		bi_cpy(&Z, B);
        if(B->sign==POSITIVE) Z->sign=NEGATIVE;
        else Z->sign=POSITIVE;
		return;
	}
	
    //B=0 -> A-0=Z
	if (bi_is_zero(B)) {
        //Z=A
		bi_cpy(&Z, A);
        return;
	}
	
    //A=B -> A-B=0
	if (bi_cmp(A,B)==0){
        //Z=0
		bi_set_zero(Z);
		return;
	}
		
	//0<A,B
    if (A->sign, B->sign == POSITIVE) {
        //0<B<A
        if (bi_cmp(A,B)==1) {
            //Z=A-B
            SUBC(A,B,Z);
            return;
        }

        //0<B<A
        else {
            //Z=-(B-A)
            SUBC(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    //0>A,B
    else if(A->sign, B->sign == NEGATIVE) {
        //0>A>B
        if (bi_cmp(A,B)==1) {
            //Z=|B|-|A|
            SUBC(B,A,Z);
            return;
        }

        //0>B>A
        else {
            //Z=-(|A|-|B|)
            SUBC(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    //A>0, B<0
    else if(A->sign==POSITIVE && B->sign==NEGATIVE) {
        //Z=A+|B|
        ADD(A,B,Z);
        return;
    }

    //A<0, B>0
    else if(A->sign==NEGATIVE && B->sign==POSITIVE) {
        //Z=-(|A|+B)
        ADD(A,B,Z);
        Z->sign=NEGATIVE;
        return;
    }
}
