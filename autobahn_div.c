#include "autobahn.h"

/****************************************************************************************
 * bi_div_long
 * x=yq+r(0<=r<y)
 * 
 * input
 * q: quotient
 * r: remainder
 * x: dividend
 * y: divisor
 ****************************************************************************************/
void bi_div_long(bi** q, bi** r, bi* x, bi* y)
{
    //if y <= 0 or x < 0, return INVALID
    if ((bi_is_zero(y) && y->sign == 1)|| x->sign == 1)
    {
        exit(1);
    }

    //if x < y, return q = 0, r = x
    if (bi_cmp(x,y) == -1)
    {
        bi_set_zero(q);
        bi_cpy(r,x);
        return;
    }

    //if y = 1, return q = x, r = 0
    if(bi_is_one(y))
    {
        bi_cpy(q,x);
        bi_set_zero(r);
        return;
    }

    //q_temp : temporary quotient
    //r_temp : temporary remainder
    bi* q_temp = NULL;
    bi* r_temp = NULL;
    bi_new(&q_temp, (x->dmax)-(y->dmax)+1);
    bi_new(&r_temp, y->dmax);
    bi_set_zero(&q_temp);
    bi_set_zero(&r_temp);

    //bi_two : bi storing 2
    bi* bi_two = NULL;
    bi_new(&bi_two, 1);
    bi_two->a[0]=0x02;

    //two_squ : bi storing 2^j
    bi* two_squ=NULL;
    bi_new(&two_squ, (x->dmax)-(y->dmax)+1);
    two_squ->a[0]=0x01;

    //x_bit : bi storing 1 bit of x
    bi* x_bit = NULL;
    bi_new(&x_bit, 1);

    //Divide x into blocks of 32 bits and calculate
    for(int block_idx=x->dmax-1; block_idx>=0; block_idx--)
    {
        //Calculate bitwise
        for(int bit_idx=32-1; bit_idx>=0; bit_idx--)
        {
            //store bit_idx-th bit of x into x_bit
            x_bit->a[0]=(x->a[block_idx] & (1 << bit_idx)) >> bit_idx;

            //remainder = remainder * 2 + x_bit
            bi_mul_improved(&r_temp,r_temp,bi_two);
            bi_add(&r_temp,r_temp,x_bit);

            if(bi_cmp(r_temp,y)>=0) //if remainder >= divisor
            {
                //store 2^bit_idx into two_squ
                for(int j=bit_idx+32*block_idx; j>0; j--)
                {
                    bi_mul_improved(&two_squ,two_squ,bi_two);
                }
                //quotient = quotient + 2^bit_idx
                bi_add(&q_temp,q_temp,two_squ);

                //remainder = remainder - divisor
                bi_sub(&r_temp,r_temp,y);

                //initialize two_squ
                bi_set_zero(&two_squ);
                two_squ->a[0]=0x01;
            }
		}
	}

    //refine quotient and remainder to remove leading zeros
    bi_refine(q_temp);
    bi_refine(r_temp);

    //copy quotient and remainder to output variables q and r
    bi_cpy(q,q_temp);
    bi_cpy(r,r_temp);

    //Delete the temporary storage
    bi_delete(&q_temp);
    bi_delete(&r_temp);
    bi_delete(&bi_two);
    bi_delete(&two_squ);
    bi_delete(&x_bit);
    
    return;
}