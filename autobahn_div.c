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
        bi_new(&rr, y->dmax+1);

        for(int block_idx=x->dmax-1; block_idx>=0; block_idx--){
                for(int bit_idx=32-1; bit_idx>=0; bit_idx--){
                        uint32_t aj=(x->a[block_idx] & (1 << bit_idx)) >> bit_idx;
                        rr->a[block_idx]=(rr->a[block_idx]<<1)^aj;
                        if(bi_cmp(rr,y)>=0){
                                qq->a[block_idx]=qq->a[block_idx]^(1<<bit_idx);
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