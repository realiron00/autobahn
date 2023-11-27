#include "autobahn_test.h"

/* read integer from file */
static void read_integer_from_file(Bigint** bigint, FILE* testvector_file)
{
    char buffer[TEST_VECTOR_SIZE] = {0};

    fscanf(testvector_file, "%s", buffer);

   	bigint_set_by_hex_string(bigint, buffer, POSITIVE);
}

/* write integer to file */
static void write_integer_into_file(FILE* file, Bigint* x)
{
    if (x->sign) 
		fprintf(file, "-");
    
    // write most significant position of bigint integer: %08x is not allowed.
    fprintf(file, PRINT_WORD_FORMAT, x->digits[x->digit_num - 1]);

	// write bigint integer.
    for (int i = x->digit_num - 2; i >= 0; i--)
        fprintf(file, PRINT_WORD_FIX_FORMAT, x->digits[i]);

    fprintf(file, "\n");
}

/* test code */
void bigint_test()
{
    FILE* f_operand_x = fopen(OPERAND_X_FILE_PATH, "r");
    FILE* f_operand_y = fopen(OPERAND_Y_FILE_PATH, "r");
    FILE* f_result = fopen(RESULT_FILE_PATH, "w");

    if(f_operand_x == NULL || f_operand_y == NULL) {
        perror("bigint_test: file open error");
        exit(1);
    }

    Bigint* operand_x = NULL;
    Bigint* operand_y = NULL;
    Bigint* result = NULL;

    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&result, 1);

    uint32_t limit = 4225;

    /*-----------------------------------------------------------*/
	while(limit--)
	{
        /* 정수 읽어오기 */
    	read_integer_from_file(&operand_x, f_operand_x);
        read_integer_from_file(&operand_y, f_operand_y);

        /* 부호 바꾸기: 바꾸기 싫으면 주석처리 */
        if(bigint_is_zero(operand_x) == FALSE) operand_x->sign = NEGATIVE;
        if(bigint_is_zero(operand_y) == FALSE) operand_y->sign = NEGATIVE;

        /* 연산 */
    	bigint_multiplication_karatsuba(&result, operand_x, operand_y);

        /* 연산 결과 저장 */
        write_integer_into_file(f_result, result);
	}
    /*-----------------------------------------------------------*/

    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
	bigint_delete(&result);

    fclose(f_operand_x);
    fclose(f_operand_y);
    fclose(f_result);
}

/* test code */
void bigint_test_div()
{
    FILE* f_operand_x = fopen(OPERAND_X_FILE_PATH, "r");
    FILE* f_operand_y = fopen(OPERAND_Y_FILE_PATH, "r");
    FILE* f_result_q = fopen(RESULT_FILE_PATH_Q, "w");
    FILE* f_result_r = fopen(RESULT_FILE_PATH_R, "w");

    if(f_operand_x == NULL || f_operand_y == NULL) {
        perror("bigint_test: file open error");
        exit(1);
    }

    Bigint* operand_x = NULL;
    Bigint* operand_y = NULL;
    Bigint* quotient = NULL;
    Bigint* remainder = NULL;

    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&quotient, 1);
    bigint_new(&remainder, 1);

    /*-----------------------------------------------------------*/
	while(!feof(f_operand_x))
	{
        /* 정수 읽어오기 */
    	read_integer_from_file(&operand_x, f_operand_x);
        read_integer_from_file(&operand_y, f_operand_y);

        /* 부호 바꾸기: 바꾸기 싫으면 주석처리 */
        //operand_x->sign = NEGATIVE;
        //operand_y->sign = NEGATIVE;

        if(bigint_is_zero(operand_y) == TRUE) {
            fprintf(f_result_q, "DIV0!\n");
            fprintf(f_result_r, "DIV0!\n");
        } else {
            /* 연산 */
    	    bigint_division_word_long(&quotient, &remainder, operand_x, operand_y);

            /* 연산 결과 저장 */
            write_integer_into_file(f_result_q, quotient);
            write_integer_into_file(f_result_r, remainder);
        }
        
	}
    /*-----------------------------------------------------------*/

    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
	bigint_delete(&quotient);
    bigint_delete(&remainder);

    fclose(f_operand_x);
    fclose(f_operand_y);
    fclose(f_result_q);
    fclose(f_result_r);
}