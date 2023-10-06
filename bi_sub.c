#include "autobahn.h"

//x-y=z
void bi_sub(bi* x, bi* y, bi* z)
{
	if (bi_is_zero(x)) {
		bi_cpy(&z, y);
        if(y->sign==POSITIVE) z->sign=NEGATIVE;
        else z->sign=POSITIVE;
		return;
	}
	
	if (bi_is_zero(y)) {
		bi_cpy(&z, x);
        return;
	}
	
	if (bi_cmp(x,y)==0){
		bi_set_zero(z);
		return;
	}
		
	
	
}