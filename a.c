        for(int block_idx=x->dmax-1; block_idx>=0; block_idx--)
        {
                for(int bit_idx=32-1; bit_idx>=0; bit_idx--)
                {
                        uint32_t bit = (x->a[block_idx] & (1 << bit_idx)) >> bit_idx;

                        R->a[block_idx] = (R->a[block_idx] << 1) ^ bit; //! R 전체를 1칸 시프트 하는 함수 작성 필요 //! bi_mul(R, 2);

                        if(bi_cmp(R,y)>=0)
                        {
                                Q->a[block_idx]=Q->a[block_idx]^(1<<bit_idx); //! bi_add(Q, 2^j);
                                bi_sub(&R,R,y);
                        }
                }
        }