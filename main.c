#include "autobahn.h"

//autobahn_common test
void test1()
{
	bi* x = NULL;
	bi* y = NULL;
	
	//[1] bi_new 함수 테스트
	bi_new(&x, 8);
	printf("1. Created big integer\n");
	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) 
	{
		printf("%d ", x->a[i]);
	}
	printf("\n\n");

	//[2] x.a에 큰 수 삽입
	for (int i = 0; i < 6; i++)
	{
		x->a[i] = i;
	}
	printf("2. insert big integer\n");
	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) 
	{
		printf("%d ", x->a[i]);
	}
	printf("\n\n");

	//[3] bi_refine 함수 테스트
	bi_refine(x);
	printf("3. Refined big integer\n");
	printf("x Dmax after refining: %d\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) 
	{
		printf("%d ", x->a[i]);
	}
	printf("\n/n");

	//[4] bi_cpy 함수 테스트
	bi_cpy(&y, x);
	printf("4. copy big integer\n");
	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) 
	{
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

	//[5] bi_delete 함수 테스트
	bi_delete(&x);
	if (x == NULL)
	printf("5. Big integer deleted successfully.\n");
}

//autobahn_sub test
void test2()
{
	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 2);
    bi_new(&y, 2);

    x->a[0]=5678;
	x->a[1]=1234;

    y->a[0]=4567;
	y->a[1]=623;

    printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n\n");

	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

    bi* z = NULL;
	bi_new(&z, 1);
	bi_sub(&z,x,y);

	printf("z Sign: %u\n", z->sign);
	printf("z Dmax: %u\n", z->dmax);
	printf("z Digits: ");
	for (int i = 0; i < z->dmax; i++) {
		printf("%d ", z->a[i]);
	}
}

//autobahn_div test
void test3()
{
	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 1);
    bi_new(&y, 1);

    x->a[0]=0x3ff;

	y->a[0]=0x34ffff;

	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n \n");

	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

	bi* q = NULL;
	bi_new(&q, 1);

	bi* r = NULL;
	bi_new(&r, 1);

	bi_div_long(&q, &r, x, y);

	printf("q Sign: %u\n", q->sign);
	printf("q Dmax: %u\n", q->dmax);
	printf("q Digits: ");
	for (int i = 0; i < q->dmax; i++) {
		printf("%d ", q->a[i]);
	}
	printf("\n\n");

	printf("r Sign: %u\n", r->sign);
	printf("r Dmax: %u\n", r->dmax);
	printf("r Digits: ");
	for (int i = 0; i < r->dmax; i++) 
	{
		printf("%d ", r->a[i]);
	}
}

//autobahn_mul test
void test4()
{
	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 2);
    bi_new(&y, 1);

    x->a[0]=2345;

	y->a[0]=2;

	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n \n");

	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

	bi* z = NULL;
	bi_new(&z, 2);

	bi_mul_improved(&z, x, y);

	printf("z Sign: %u\n", z->sign);
	printf("z Dmax: %u\n", z->dmax);
	printf("z Digits: ");
	for (int i = 0; i < z->dmax; i++) {
		printf("%d ", z->a[i]);
	}
}

//autobahn_mul 지수승 test
void test5()
{
	bi* x = NULL;
	bi* x2 = NULL;

	bi_new(&x, 1);
	bi_new(&x2, 1);
	x->a[0]=0x01;
	x2->a[0]=0x02;

	for(int j=10; j>0; j--){
		bi_mul_improved(&x,x,x2);
	}
	printf("x dmax: %d\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%x ", x->a[i]);
	}
	printf("\n");
}

//autobahn_div_long 동현 test
void test6()
{
	bi* operand_x = NULL;
    bi* operand_y = NULL;
    bi* result = NULL;
    bi* quotient = NULL;
    bi* remainder = NULL;

    bi_new(&operand_x, 1);
    bi_new(&operand_y, 1);
    bi_new(&result, 1);
    bi_new(&quotient, 1);
    bi_new(&remainder, 1);

    bi_set_by_str(&operand_x, "3ffffffff", POSITIVE, HEXADECIMAL);
    bi_set_by_str(&operand_y, "3", POSITIVE, HEXADECIMAL);

    bi_show_hex(operand_x);
    bi_show_hex(operand_y);

    bi_div_long(&quotient, &remainder, operand_x, operand_y);
    bi_show_hex(quotient);
    bi_show_hex(remainder);

    bi_delete(&operand_x);
    bi_delete(&operand_y);
    bi_delete(&result);
    bi_delete(&quotient);
    bi_delete(&remainder);
}

void test7()
{
	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 1);
    bi_new(&y, 1);

    x->a[0]=0x3ff;

	y->a[0]=0x34;

	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n \n");

	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

	bi* q = NULL;
	bi_new(&q, 1);

	bi* r = NULL;
	bi_new(&r, 1);

	bi_div_naive(&q, &r, x, y);

	printf("q Sign: %u\n", q->sign);
	printf("q Dmax: %u\n", q->dmax);
	printf("q Digits: ");
	for (int i = 0; i < q->dmax; i++) {
		printf("%d ", q->a[i]);
	}
	printf("\n\n");

	printf("r Sign: %u\n", r->sign);
	printf("r Dmax: %u\n", r->dmax);
	printf("r Digits: ");
	for (int i = 0; i < r->dmax; i++) 
	{
		printf("%d ", r->a[i]);
	}
}

int main()
{
	// int num;
	// printf("1. autobahn_common test\n");
	// printf("2. autobahn_sub test\n");
	// printf("3. autobahn_div test\n");
	// printf("4. autobahn_mul test\n");
	// printf("5. autobahn_mul 지수승 test\n");
	// printf("6. autobahn_div_long 동현 test\n");
	// printf("7. autobahn_div_naive test\n");
	// printf("8. autobahn_test\n");
	// printf("9. autobahn_div_long test\n");
	// printf("select number: ");
	// scanf("%d", &num);
	// switch(num)
	// {
	// 	case 1:
	// 		test1();
	// 		break;
	// 	case 2:
	// 		test2();
	// 		break;
	// 	case 3:
	// 		test3();
	// 		break;
	// 	case 4:
	// 		test4();
	// 		break;
	// 	case 5:
	// 		test5();
	// 		break;	
	// 	case 6:
	// 		test6();
	// 		break;
	// 	case 7:
	// 		test7();
	// 		break;	
	// 	case 8:
	// 		bi_test();
	// 		break;
	// 	case 9:
	// 		bi_test_div();
	// 		break;
	// 	default:
	// 		printf("wrong number\n");
	// 		break;
	// }

	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 1);
    bi_new(&y, 1);

    x->a[0]=0x3fff;

	y->a[0]=0x34;

	printf("x Sign: %u\n", x->sign);
	printf("x Dmax: %u\n", x->dmax);
	printf("x Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n \n");

	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n\n");

	bi* q = NULL;
	bi_new(&q, 1);

	bi* r = NULL;
	bi_new(&r, 1);

	//bi_div_long(&q, &r, x, y);
	bi_div_general_long(&q, &r, x, y);

	printf("q Sign: %u\n", q->sign);
	printf("q Dmax: %u\n", q->dmax);
	printf("q Digits: ");
	for (int i = 0; i < q->dmax; i++) {
		printf("%d ", q->a[i]);
	}
	printf("\n\n");

	printf("r Sign: %u\n", r->sign);
	printf("r Dmax: %u\n", r->dmax);
	printf("r Digits: ");
	for (int i = 0; i < r->dmax; i++) 
	{
		printf("%d ", r->a[i]);
	}



	// bi* x = NULL;
    // bi* y = NULL;

    // bi_new(&x, 1);
    // bi_new(&y, 1);

    // x->a[0]=0x3fff;

	// y->a[0]=0x34;

	// printf("x Sign: %u\n", x->sign);
	// printf("x Dmax: %u\n", x->dmax);
	// printf("x Digits: ");
	// for (int i = 0; i < x->dmax; i++) {
	// 	printf("%d ", x->a[i]);
	// }
	// printf("\n \n");

	// printf("y Sign: %u\n", y->sign);
	// printf("y Dmax: %u\n", y->dmax);
	// printf("y Digits: ");
	// for (int i = 0; i < y->dmax; i++) {
	// 	printf("%d ", y->a[i]);
	// }
	// printf("\n\n");

	// bi* q = NULL;
	// bi_new(&q, 1);

	// bi* r = NULL;
	// bi_new(&r, 1);

	// bi_div_long(&q, &r, x, y);

	// printf("q Sign: %u\n", q->sign);
	// printf("q Dmax: %u\n", q->dmax);
	// printf("q Digits: ");
	// for (int i = 0; i < q->dmax; i++) {
	// 	printf("%d ", q->a[i]);
	// }
	// printf("\n\n");

	// printf("r Sign: %u\n", r->sign);
	// printf("r Dmax: %u\n", r->dmax);
	// printf("r Digits: ");
	// for (int i = 0; i < r->dmax; i++) 
	// {
	// 	printf("%d ", r->a[i]);
	// }

	return 0;
}