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
        bi_new(&qq, x->dmax);
        bi_new(&rr, x->dmax);

        for(int i=x->dmax-1; i>=0; i--){
                for(int j=32-1; j>=0; j--){
                        uint32_t aj=(x->a[i] & (1 << j)) >> j;
                        rr->a[i]=(rr->a[i]<<1)^aj;
                        if(bi_cmp(rr,y)>=0){
                                qq->a[i]=qq->a[i]^(1<<j);
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