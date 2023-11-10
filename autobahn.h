#ifndef AUTOBAHN_H
#define AUTOBAHN_H

#include "autobahn_common.h"

void bi_add(bi** z, bi* x, bi* y);
void bi_sub(bi** z, bi* x, bi* y);
void bi_div_long(bi** q, bi** r, bi* x, bi* y);
void bi_mul_improved(bi** z, bi* x, bi* y);
void bi_mul_karatsuba(bi** z, bi* x, bi* y);

#endif