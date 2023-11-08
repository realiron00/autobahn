#include "autobahn.h"

//autobahn_common test
void test1()
{
	bi* x = NULL;
	bi* y = NULL;
	
	//[1] bi_new 함수 테스트
	bi_new(&x, 8);
	printf("1. Created big integer\n");
	printf("Sign: %u\n", x->sign);
	printf("Dmax: %u\n", x->dmax);
	printf("Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n");

	//[2] x.a에 큰 수 삽입
	for (int i = 0; i < 6; i++)
	{
		x->a[i] = i;
	}
	printf("2. insert big integer\n");
	printf("Sign: %u\n", x->sign);
	printf("Dmax: %u\n", x->dmax);
	printf("Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n");

	//[3] bi_refine 함수 테스트
	bi_refine(x);
	printf("3. Refined big integer\n");
	printf("Dmax after refining: %d\n", x->dmax);
	printf("Digits: ");
	for (int i = 0; i < x->dmax; i++) {
		printf("%d ", x->a[i]);
	}
	printf("\n");

	//[4] bi_cpy 함수 테스트
	bi_cpy(&y, x);
	printf("4. copy big integer\n");
	printf("Sign: %u\n", y->sign);
	printf("Dmax: %u\n", y->dmax);
	printf("Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n");

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
	printf("\n");
	printf("y Sign: %u\n", y->sign);
	printf("y Dmax: %u\n", y->dmax);
	printf("y Digits: ");
	for (int i = 0; i < y->dmax; i++) {
		printf("%d ", y->a[i]);
	}
	printf("\n");
    bi* z = NULL;
	bi_new(&z, 1);
	bi_sub(&z,x,y);

	printf("z Sign: %u\n", z->sign);
	printf("z Dmax: %u\n", z->dmax);
	printf("z Digits: ");
	for (int i = 0; i < z->dmax; i++) {
		printf("%d ", z->a[i]);
	}
	printf("\n\n");
}

void test3()
{
	bi* x = NULL;
    bi* y = NULL;

    bi_new(&x, 2);
    bi_new(&y, 1);
    x->a[0]=555555555;
    x->a[1]=1234;
	y->a[0]=4567;
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
	bi_new(&q, 2);
	bi* r = NULL;
	bi_new(&r, 2);
	bi_long_div(&q, &r, x, y);

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
	for (int i = 0; i < r->dmax; i++) {
		printf("%d ", r->a[i]);
	}
}

int main()
{
	int num=3;
	switch(num)
	{
		case 1:
			test1();
			break;
		case 2:
			test2();
			break;
		case 3:
			test3();
			break;
		default:
			printf("wrong number\n");
			break;
	}
	return 0;
}