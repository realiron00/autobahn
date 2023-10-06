#include "autobahn_common.h"

/**************************************************************
*	구조체를 삭제하는 함수
*
*	x : 삭제되는 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
/*
void bi_delete(bi** x)
{
	//구조체가 이미 비어있을 경우, 반환
	if (*x == NULL)
		return;

	//구조체의 메모리를 해제
	free((*x)->a);
	free(*x);

	//댕글링 포인터를 방지하기 위해 NULL값을 삽입
	*x = NULL;
}
*/

/**************************************************************
*	구조체를 생성하는 함수
*
*	x : 생성하는 빅넘버 구조체 주소를 담을 공간.
*   dmax : 생성하는 빅넘버 구조체의 배열의 길이.
**************************************************************/
/*
void bi_new(bi** x, bi_word dmax)
{
	//구조체에 어떤 값이 있을 경우, 해당 값을 삭제
	if (*x != NULL)
		bi_delete(x);

	//구조체 x의 메모리 할당
	*x = (bi*)malloc(sizeof(bi));
	(*x)->sign = POSITIVE;
	(*x)->dmax = dmax;

	//배열의 길이만큼 x.a의 메모리 할당
	(*x)->a = (bi_word*)calloc(dmax, sizeof(bi_word));
}
*/

/**************************************************************
*	큰 수의 뒷부분에 있는 0값을 제거하는 함수
*
*	x : 0값을 제거할 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
/*
void bi_refine(bi* x)
{
	//구조체가 비어있을 경우, 반환
	if (x == NULL)
		return;

	//0이 아닌 부분의 배열 번호를 찾아냄
	bi_word new_dmax = x->dmax;

	while (new_dmax > 1) { //배열의 끝부분부터 0이 아닌 값이 나올 때까지 확인
		if (x->a[new_dmax - 1] != 0) //0이 아닌 값이 나온 배열 번호를 new_dmax에 저장
			break;
		new_dmax--;
	}

	//0이 아닌 부분만 메모리 재할당
	if (x->dmax != new_dmax) {
		x->dmax = new_dmax;
		x->a = (bi_word*)realloc(x->a, sizeof(bi_word) * new_dmax);
	}

	//a의 값이 0일 경우, a를 0으로 정함
	if ((x->dmax == 1) && (x->a[0] == 0)) {
		x->sign = POSITIVE;
	}
}
*/

/**************************************************************
*	구조체 y에 구조체 x를 복사해주는 함수
*
*	y : 복사된 빅넘버 구조체 주소를 담을 공간.
*   x : 복사할 빅넘버 구조체 주소를 담을 공간.
**************************************************************/
/*
void bi_cpy(bi** y, const bi* x)
{
	//구조체 y에 어떤 값이 있을경우, 해당 값을 삭제
	if (*y != NULL)
		bi_delete(y);

	//구조체 x의 배열의 길이만큼 구조체를 생성
	bi_new(y, x->dmax);

	//구조체 y에 x의 데이터 복사
	(*y)->sign = x->sign;
	for (int i = 0; i < x->dmax; i++) {
		(*y)->a[i] = x->a[i];
	}
}
*/

/**************************************************************
*   두 큰 음이 아닌 정수의 대수비교를 해주는 함수
*
*	x, y : 비교할 빅넘버 구조체 주소를 담을 공간.
*   반환값 : x가 y보다 크다면 1, x와 y가 같다면 0, x가 y보다 작다면 -1을 반환.
**************************************************************/
/*
static bi_word bi_cmpABS(const bi* x, const bi* y)
{
	bi_word x_dmax = x->dmax;
	bi_word y_dmax = y->dmax;

	//x의 자릿수가 y보다 크다면, 1을 반환
	if (x_dmax > y_dmax)
		return 1;
	
	//y의 자릿수가 x보다 크다면, -1을 반환
	else if (x_dmax < y_dmax)
		return -1; //! bi_word가 uint32_t(unsigned int)같은 자료형이라서 음수가 리턴될 수가 없음. --> 함수 리턴 자료형을 int 자료형으로 바꿔야 함.

	//자릿수가 같을 때, 각 자리의 값을 비교
	for (int i = x_dmax-1; i >= 0; i--) {
		//자리의 값이 x가 크다면, 1을 반환
		if (x->a[i] > y->a[i])
			return 1;

		//자리의 값이 y가 크다면, -1을 반환
		else if (x->a[i] < y->a[i])
			return -1;
	}
	//모든 자리의 값이 같다면, 0을 반환
	return 0;
}
*/

/**************************************************************
*	두 큰 정수의 대수비교를 해주는 함수
*
*	x, y : 비교할 빅넘버 구조체 주소를 담을 공간.
*
*   반환값 : x가 y보다 크다면 1, x와 y가 같다면 0, x가 y보다 작다면 -1을 반환.
**************************************************************/
/*
bi_word bi_cmp(const bi* x, const bi* y) 
{
	//x가 양수고 y가 음수라면, 1을 반환
	if (x->sign == POSITIVE && y->sign == NEGATIVE)
		return 1;

	//x가 음수고 y가 양수라면, -1을 반환
	if (x->sign == NEGATIVE && y->sign == POSITIVE)
		return -1;

	//x와 y의 절댓값을 비교
	bi_word ret = bi_cmpABS(x, y);
	
	//두 수가 양수라면, 절댓값을 비교한대로 반환
	if (x->sign == POSITIVE)
		return ret;

	//두 수가 음수라면, 절댓값을 비교한 것의 반대로 반환
	else
		return ret * (-1);
}
*/