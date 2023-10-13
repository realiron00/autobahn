#include "autobahn.h"


/*
Textbook
 procedure MULC(A, B)
2: C ← 0
3: for j = 0 to n − 1 do
4: for i = 0 to m − 1 do
5: T ← AjBi . 0 ≤ T < W2
6: T ← T << w(i + j)
7: C ← ADDC(C, T)
8: end for
9: end for
10: return C
11: end procedure
*/
void bi_mul(bi* z, bi* x, bi* y)
{
    bi_new(&z, x->dmax+y->dmax);

    bi_word* t;
    for(int j=0;j<y->dmax;j++){
        for(int i=0;i<x->dmax;i++){
            t = (bi_word*)malloc(sizeof(bi_word));
            *t = x->a[i]*y->a[j];
            *t = *t << (i+j);
            bi_add(z, z, t);
            free(t);
        }
    }
}