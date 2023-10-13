#include "autobahn.h"

void bi_mul(bi* z, bi* x, bi* y)
{
    bi_new(&z, x->dmax+y->dmax);
}