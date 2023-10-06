#include "autobahn_common.h"

// show base_hex
void bi_show_hex(const bi* x) {

	if (x->sign) {
		printf("-");
	}
	printf("0x");

	for (int i = x->dmax-1; i >= 0; i--) {
		printf("%08x", x->a[i]);
	}
	printf("\n");
}


// show base_bin
void bi_show_bin(const bi* x) {
	for (int i = x->dmax-1; i >= 0; i--) {
		for (int j = 31; j >= 0; j--) {
			printf("%d", (x->a[i] >> j) & 1);
		}
		printf("\n");
	}
}

// set array
bi_word bi_set_by_arr(bi** x, const bi_word* a, sign sign, bi_word dmax){
	bi_new(x, dmax);  // 새로운 구조체를 생성하고
	(*x)->sign = sign; // 부호 저장
	for (int i = 0; i<dmax;i++){ // 반복문을 돌리면서 array 배열을 새로운 구조체에 저장
		(*x)->a[i] = a[i];
	}
	return 0;
}

bi_word bi_set_by_str(bi** x, const char* str, sign sign, base base) {
    int length = strlen(str); // 문자열의 길이
    int dmax = (length + 7) / 8; // 8개의 16진수 문자를 4바이트로 압축 

    if (base != 16) {
        printf("지원하지 않는 진법입니다.");
        return -1;
	}

    bi_new(x, dmax);

    (*x)->sign = sign;

    for (int i = 0; i < dmax; i++) {
        char temp[9]; // 8개의 16진수 문자를 저장할 임시 배열 (8글자 + NULL 종료 문자)
        int start = i * 8;
        int end = start + 8;
        if (end > length) {
            end = length;
        }
        strncpy(temp, str + start, end - start); // 문자열을 8글자씩 잘라서 temp에 복사
        temp[end - start] = '\0';

        // strtoul을 이용하여 구조체 배열에 저장
        (*x)->a[i] = strtoul(temp, NULL, base);
    }

    return 0;
}

void bi_set_one(bi** x){ // Set One
    bi_new(x,1); // 
    (*x)->sign = 0; // 부호삭제
    (*x)->a[0] = 1; // 비트 마스킹
}

//구조체를 삭제하는 함수
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

//구조체를 생성하는 함수
void bi_new(bi** x, bi_word dmax)
{
	//구조체에 어떤 값이 있을 경우, 해당 값을 삭제
	if (*x != NULL)
		bi_delete(x);

	//구조체 x의 메모리 할당
	*x = (bi*)malloc(sizeof(bi));
	(*x)->sign = 0;
	(*x)->dmax = dmax;

	//배열의 길이만큼 x.a의 메모리 할당
	(*x)->a = (bi_word*)calloc(dmax, sizeof(bi_word));
}

//큰 수의 뒷부분에 있는 0값을 제거하는 함수
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
		x->sign = 0;
	}
}

//구조체 y에 구조체 x를 복사해주는 함수
void bi_cpy(bi** y, const bi* x)
{
	//구조체 y에 어떤 값이 있을경우, 해당 값을 삭제
	if (*y != NULL)
		bi_delete(y);

	//구조체 x의 배열의 길이만큼 구조체를 생성
	bi_new(y, x->dmax);

	//구조체 y에 x의 데이터 복사
	(*y)->sign = x->sign;
	for (bi_word i = 0; i < x->dmax; i++) {
		(*y)->a[i] = x->a[i];
	}
}

//두 큰 음이 아닌 정수의 대수비교를 해주는 함수
/*
x가 y보다 크다면:	1을 반환
x와 y가 같다면:		0을 반환
x가 y보다 작다면:	-1을 반환
*/
bi_word bi_cmpABS(const bi* x, const bi* y)
{
	bi_word x_dmax = x->dmax;
	bi_word y_dmax = y->dmax;

	//x의 자릿수가 y보다 크다면, 1을 반환
	if (x_dmax > y_dmax)
		return 1;
	
	//y의 자릿수가 x보다 크다면, -1을 반환
	else if (x_dmax < y_dmax)
		return -1;

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

//두 큰 정수의 대수비교를 해주는 함수
/*
x가 y보다 크다면:	1을 반환
x와 y가 같다면:		0을 반환
x가 y보다 작다면:	-1을 반환
*/
bi_word bi_cmp(const bi* x, const bi* y) 
{
	//x가 양수고 y가 음수라면, 1을 반환
	if (x->sign == 0 && y->sign == 1)
		return 1;

	//x가 음수고 y가 양수라면, -1을 반환
	if (x->sign == 1 && y->sign == 0)
		return -1;

	//x와 y의 절댓값을 비교
	bi_word ret = bi_cmpABS(x, y);
	
	//두 수가 양수라면, 절댓값을 비교한대로 반환
	if (x->sign == 0)
		return ret;

	//두 수가 음수라면, 절댓값을 비교한 것의 반대로 반환
	else
		return ret * (-1);
}

// is zero
bool bi_is_zero(const bi* x){ // Is Zero? - case :mul
	if (x->sign == 1 || x->a[0] !=0 ) // 음수 부호가 있거나 값이 있으면
		return false;
	return true;
}


// is one
bool bi_is_one(const bi* x){ // Is One? - case : add 
	if (x->sign == 1 || x->a[0] !=0 ) // 음수 부호가 있거나 블럭단위로 저장했을떄 값이 있으면 False
		return false;
	return true; // True값이 나오면 add션 할때 그냥 값을 리턴
}


// Set Zero
void bi_set_zero(bi** x){ 
	
    bi_new(x,1); // 배열 생성
	
    (*x)->sign = POSITIVE; // 부호 결정 
	
    (*x)->a[0] = 0; // 값 입력

}