#include "autobahn.h"
#include "autobahn_test.h"

int main()
{
    // Bigint* x = NULL;
    // Bigint* y = NULL;
    // Bigint* q = NULL;
    // Bigint* r = NULL;

    // bigint_new(&x, 1);
    // bigint_new(&y, 1);
    // bigint_new(&q, 1);
    // bigint_new(&r, 1);

    // x->digits[0]=0x65312;

    // y->digits[0]=0x33221;

    // printf("x Sign: %u\n", x->sign);
    // printf("x Dmax: %u\n", x->digit_num);
    // printf("x Digits: ");
    // for (int i = 0; i < x->digit_num; i++) {
    //     printf("0x%x ", x->digits[i]);
    // }
    // printf("\n \n");

    // printf("y Sign: %u\n", y->sign);
    // printf("y Dmax: %u\n", y->digit_num);
    // printf("y Digits: ");
    // for (int i = 0; i < y->digit_num; i++) {
    //     printf("0x%x ", y->digits[i]);
    // }
    // printf("\n\n");

    // bigint_division_word_long(&q, &r, x, y);

    // printf("q Sign: %u\n", q->sign);
    // printf("q Dmax: %u\n", q->digit_num);
    // printf("q Digits: ");
    // for (int i = 0; i < q->digit_num; i++) {
    //     printf("0x%x ", q->digits[i]);
    // }
    // printf("\n\n");

    // printf("r Sign: %u\n", r->sign);
    // printf("r Dmax: %u\n", r->digit_num);
    // printf("r Digits: ");
    // for (int i = 0; i < r->digit_num; i++) 
    // {
    //     printf("0x%x ", r->digits[i]);
    // }
    // printf("\n\n");

    // Bigint* x = NULL;
    // Bigint* y = NULL;
    // Bigint* z = NULL;
    // Bigint* w = NULL;

    // bigint_new(&x, 1);
    // bigint_new(&y, 1);
    // bigint_new(&z, 1);
    // bigint_new(&w, 1);

    // bigint_set_by_hex_string(&x, "40000000", POSITIVE);
    // bigint_set_by_hex_string(&y, "6", POSITIVE);

    // bigint_multiplication_textbook(&z, x, y);

    // bigint_show_hex(z);
    // bigint_delete(&x);
    // bigint_delete(&y);
    // bigint_delete(&z);
    // bigint_delete(&w);

    bigint_test_div();

    return 0;
}