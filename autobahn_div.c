#include "autobahn.h"

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

        bi* qq = NULL;
        bi* rr = NULL;
        bi_new(&qq, (x->dmax)-(y->dmax)+1);
        bi_new(&rr, y->dmax);
        bi_set_zero(&qq);
        bi_set_zero(&rr);

        bi* x2 = NULL;
        bi_new(&x2, 1);
        x2->a[0]=2;

        bi* jj=NULL;
        bi_new(&jj, (x->dmax)-(y->dmax)+1);
        jj->a[0]=1;

        bi* aj = NULL;
        bi_new(&aj, 1);

        for(int block_idx=x->dmax-1; block_idx>=0; block_idx--){
                for(int bit_idx=32-1; bit_idx>=0; bit_idx--){
                        aj->a[0]=(x->a[block_idx] & (1 << bit_idx)) >> bit_idx;
                        MULC(&rr,rr,x2);
                        bi_add(&rr,rr,aj);
                        if(bi_cmp(rr,y)>=0){
                                for(int j=bit_idx; j>0; j--){
                                        MULC(&jj,jj,x2);
                                }
                                printf("jjjj Digits: ");
                                for (int i = 0; i < jj->dmax; i++) {
                                        printf("%d ", jj->a[i]);
                                        }
                                bi_add(&qq,qq,jj);
                                bi_sub(&rr,rr,y);
                        }
                }
        }

        bi_refine(qq);
        bi_refine(rr);

        bi_cpy(q,qq);
        bi_cpy(r,rr);

        bi_delete(&qq);
        bi_delete(&rr);

        return;
}