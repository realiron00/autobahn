#ifndef AUTOBAHN_H
#define AUTOBAHN_H

#include "autobahn_common.h"

void bi_add(bi** z, bi* x, bi* y);
void bi_sub(bi** z, bi* x, bi* y);
void bi_long_div(bi** q, bi** r, bi* x, bi* y);
void bi_mul(bi** z, bi* x, bi* y);
void arr_mod(bi** z, int k);


#endif