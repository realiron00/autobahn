#include "autobahn_common.h"

/**************************************************************
*	구조체를 삭제하는 함수
*
*	x : 삭제되는 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void bi_delete(bi** x)
{
	//just return if the structure is already empty
	if (*x == NULL)
		return;

	//free the memory of the structure
	free((*x)->a);
	free(*x);

	//Insert NULL value to prevent dangling pointer
	*x = NULL;
}

/**************************************************************
*	구조체를 생성하는 함수
*
*	x : 생성하는 빅넘버 구조체 주소를 담을 공간.
*   dmax : 생성하는 빅넘버 구조체의 배열의 길이.
**************************************************************/
void bi_new(bi** x, bi_word dmax)
{
	//Delete the value in the structure if there is any
	if (*x != NULL)
		bi_delete(x);

	//Allocate memory for the structure x
	*x = (bi*)malloc(sizeof(bi));
	(*x)->sign = POSITIVE;
	(*x)->dmax = dmax;

	//Allocate memory for the array a
	(*x)->a = (bi_word*)calloc(dmax, sizeof(bi_word));
}

/**************************************************************
*	큰 수의 뒷부분에 있는 0값을 제거하는 함수
*
*	x : 0값을 제거할 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void bi_refine(bi* x)
{
	//just return if the structure is empty
	if (x == NULL)
		return;

	//Find the index of the first non-zero value from the end of the array
	bi_word new_dmax = x->dmax;

	//Check from the end of the array to the beginning until a non-zero value is found
	while (new_dmax > 1) {
		//Save the index of the first non-zero value to new_dmax
		if (x->a[new_dmax - 1] != 0)
			break;
		new_dmax--;
	}

	//If the index of the first non-zero value is different from the original dmax, change the dmax and reallocate the memory
	if (x->dmax != new_dmax) {
		x->dmax = new_dmax;
		x->a = (bi_word*)realloc(x->a, sizeof(bi_word) * new_dmax);
	}

	//If the value of the first non-zero value is 0, change the sign to POSITIVE(a=0)
	if ((x->dmax == 1) && (x->a[0] == 0)) {
		x->sign = POSITIVE;
	}
}

/**************************************************************
*	구조체 y에 구조체 x를 복사해주는 함수
*
*	y : 복사된 빅넘버 구조체 주소를 담을 공간.
*   x : 복사할 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
void bi_cpy(bi** y, const bi* x)
{
	//Delete the value in the structure if there is any in y
	if (*y != NULL)
		bi_delete(y);

	//Create a new structure y with the same dmax as x
	bi_new(y, x->dmax);

	//Copy the data from structure x to structure y
	(*y)->sign = x->sign;
	for (int i = 0; i < x->dmax; i++) {
		(*y)->a[i] = x->a[i];
	}
}

/**************************************************************
*   두 큰 음이 아닌 정수의 대수비교를 해주는 함수
*
*	x, y : 비교할 빅넘버 구조체 주소를 담을 공간.
*   반환값 : x가 y보다 크다면 1, x와 y가 같다면 0, x가 y보다 작다면 -1을 반환.
**************************************************************/
static bi_word bi_cmpABS(const bi* x, const bi* y)
{
	bi_word x_dmax = x->dmax;
	bi_word y_dmax = y->dmax;

	//Return 1 if x's dmax is bigger than y's dmax
	if (x_dmax > y_dmax)
		return 1;
	
	//Return -1 if y's dmax is bigger than x's dmax
	else if (x_dmax < y_dmax)
		return -1; //! bi_word가 uint32_t(unsigned int)같은 자료형이라서 음수가 리턴될 수가 없음. --> 함수 리턴 자료형을 int 자료형으로 바꿔야 함.

	//if x_dmax == y_dmax, compare each digit
	for (int i = x_dmax-1; i >= 0; i--) {
		//Return 1 if the value of the digit in x is bigger than the value of the digit in y
		if (x->a[i] > y->a[i])
			return 1;

		//Return -1 if the value of the digit in y is bigger than the value of the digit in x
		else if (x->a[i] < y->a[i])
			return -1;
	}
	//if all the digits are the same, return 0
	return 0;
}

/**************************************************************
*	두 큰 정수의 대수비교를 해주는 함수
*
*	x, y : 비교할 빅넘버 구조체 주소를 담을 공간.
*
*   반환값 : x가 y보다 크다면 1, x와 y가 같다면 0, x가 y보다 작다면 -1을 반환.
**************************************************************/
bi_word bi_cmp(const bi* x, const bi* y) 
{
	//Return 1 if x is positive and y is negative
	if (x->sign == POSITIVE && y->sign == NEGATIVE)
		return 1;

	//Return -1 if x is negative and y is positive
	if (x->sign == NEGATIVE && y->sign == POSITIVE)
		return -1;

	//Compare the absolute values of x and y
	bi_word ret = bi_cmpABS(x, y);
	
	//If x and y are both positive, return the value of the absolute value comparison
	if (x->sign == POSITIVE)
		return ret;

	//If x and y are both negative, return the opposite value of the absolute value comparison
	else
		return ret * (-1);
}