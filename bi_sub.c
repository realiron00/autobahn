#include "autobahn.h"

/**************************************************************
*   borrow를 이용한 블록별 뺄셈을 해주는 함수(A-b-B=C)(A>=B>0)
*
*   A,B : 뺄셈을 할 빅넘버 값 주소를 담을 공간.
*   b : borrow 값 주소를 담을 공간.
*   C : 뺄셈의 결과를 담을 빅넘버 값 주소를 담을 공간.
**************************************************************/
static void SUB_AbB(const bi_word* A, const bi_word* B, bi_word* b, bi_word* C)
{
    bi_word n_b=0; //n_b: next borrow

    *C = *A - *b; //A-b mod 2^32

    n_b=(*A < *b); //A<b이면 borrow 발생
    n_b+=(*C < *B); //A-b<B이면 borrow 발생

    *C-=*B; //A-b-B mod 2^32

    *b=n_b; //b=next borrow
}

/**************************************************************
*   두 큰 양의 정수의 뺄셈을 해주는 함수(A-B=Z)(A>=B>0)
* 
*   A,B : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   Z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
static void SUBC(bi* A, bi* B, bi* Z)
{
    if (B->dmax != A->dmax) {// 크기가 다르면 할당
		(B->a) = (bi_word*)realloc(B->a, (A->dmax) * sizeof(bi_word)); // B의 dmax 할당
	}
    bi_word b=0, C=0;
    bi* sub= NULL;
    bi_new(&sub, A->dmax);

    for (int j=0;j<A->dmax;j++) {
        SUB_AbB(&(A->a[j]), &(B->a[j]), &b, &C);
        sub->a[j]=C;
        C=0;
    }
    bi_refine(B);
    bi_refine(sub);
    bi_cpy(&Z, sub);
    bi_delete(&sub);
}

/**************************************************************
*	두 큰 정수의 뺄셈을 해주는 함수(A-B=Z)
*
*   A,B : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   Z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void SUB(bi* A, bi* B, bi* Z)
{
	if (bi_is_zero(A)) { //A=0 -> 0-B=Z
        //Z=-B
		bi_cpy(&Z, B);
        if(B->sign==POSITIVE) Z->sign=NEGATIVE;
        else Z->sign=POSITIVE;
		return;
	}
	if (bi_is_zero(B)) { //B=0 -> A-0=Z
        //Z=A
		bi_cpy(&Z, A);
        return;
	}
	if (bi_cmp(A,B)==0){ //A=B -> A-B=0
        //Z=0
		bi_set_zero(Z);
		return;
	}
	
    if (A->sign, B->sign == POSITIVE) { //0<A,B
        if (bi_cmp(A,B)==1) { //0<B<A
            //Z=A-B
            SUBC(A,B,Z);
            return;
        }
        else { //0<B<A
            //Z=-(B-A)
            SUBC(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    else if(A->sign, B->sign == NEGATIVE) { //0>A,B
        if (bi_cmp(A,B)==1) { //0>A>B
            //Z=|B|-|A|
            SUBC(B,A,Z);
            return;
        }
        else { //0>B>A
            //Z=-(|A|-|B|)
            SUBC(B,A,Z);
            Z->sign=NEGATIVE;
            return;
        }
    }

    else if(A->sign==POSITIVE && B->sign==NEGATIVE) { //A>0, B<0
        //Z=A+|B|
        ADD(A,B,Z);
        return;
    }

    else if(A->sign==NEGATIVE && B->sign==POSITIVE) { //A<0, B>0
        //Z=-(|A|+B)
        ADD(A,B,Z);
        Z->sign=NEGATIVE;
        return;
    }
}
