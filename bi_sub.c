#include "autobahn.h"

/**************************************************************
*   borrow를 이용한 블록별 뺄셈을 해주는 함수(A-b-B=C)(A>=B>0)
*
*   x,y : 뺄셈을 할 빅넘버 값 주소를 담을 공간.
*   b : borrow 값 주소를 담을 공간.
*   z : 뺄셈의 결과를 담을 빅넘버 값 주소를 담을 공간.
**************************************************************/
static void sbb(bi_word* z, bi_word* b, const bi_word* x, const bi_word* y)
{
    bi_word n_b=0; //n_b: next borrow

    *z = *x - *b; //x-b mod 2^32

    n_b=(*x < *b); //x<b이면 borrow 발생
    n_b+=(*z < *y); //x-b<y이면 borrow 발생

    *z-=*y; //x-b-y mod 2^32

    *b=n_b; //b=next borrow
}

/**************************************************************
*   두 큰 양의 정수의 뺄셈을 해주는 함수(A-B=Z)(A>=B>0)
* 
*   x,y : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
static void usub(bi* z, bi* x, bi* y)
{
    //If dmax of y is different from that of x, allocate
    if (y->dmax != x->dmax) {
		(y->a) = (bi_word*)realloc(y->a, (x->dmax) * sizeof(bi_word)); // B의 dmax 할당
	}
    bi_word b=0, d=0;
    bi* sub= NULL;
    bi_new(&sub, x->dmax);

    for (int j=0;j<x->dmax;j++) {
        sbb(&d, &b, &(x->a[j]), &(y->a[j]));
        sub->a[j]=d;
        d=0;
    }
    bi_refine(y);
    bi_refine(sub);
    bi_cpy(&z, sub);
    bi_delete(&sub);
}

/**************************************************************
*	두 큰 정수의 뺄셈을 해주는 함수(x-y=z)
*
*   x,y : 뺄셈을 할 빅넘버 구조체 주소를 담을 공간.
*   Z : 뺄셈의 결과를 담을 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void bi_sub(bi* z, bi* x, bi* y)
{
    //x=0 -> 0-y=z
	if (bi_is_zero(x)) {
        //z=-y
		bi_cpy(&z, y);
        if(y->sign==POSITIVE) z->sign=NEGATIVE;
        else z->sign=POSITIVE;
		return;
	}
    //y=0 -> x-0=z
	if (bi_is_zero(y)) {
        //z=x
		bi_cpy(&z, x);
        return;
	}
    //x=y -> x-y=0
	if (bi_cmp(x,y)==0){
        //z=0
		bi_set_zero(&z);
		return;
	}
	
    //0<x,y
    if (x->sign, y->sign == POSITIVE) {
        //0<y<x
        if (bi_cmp(x,y)==1) {
            //z=x-y
            usub(z,x,y);
            return;
        }
        //0<y<x
        else { //0<y<x
            //z=-(y-x)
            usub(z,y,x);
            z->sign=NEGATIVE;
            return;
        }
    }

    //0>x,y
    else if(x->sign, y->sign == NEGATIVE) {
        //0>x>y
        if (bi_cmp(x,y)==1) {
            //z=|y|-|x|
            usub(z,y,x);
            return;
        }
        //0>y>x
        else {
            //z=-(|x|-|y|)
            usub(y,x,z);
            z->sign=NEGATIVE;
            return;
        }
    }

    //x>0, y<0
    else if(x->sign==POSITIVE && y->sign==NEGATIVE) {
        //z=x+|y|
        bi_add(z,x,y);
        return;
    }

    //x<0, y>0
    else if(x->sign==NEGATIVE && y->sign==POSITIVE) {
        //z=-(|x|+y)
        bi_add(z,x,y);
        z->sign=NEGATIVE;
        return;
    }
}