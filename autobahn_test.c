#include "autobahn_test.h"
#include "autobahn.h"

/* read integer from file */
static void read_integer_from_file(bi** big_integer, FILE* testvector_file)
{
    char buffer[TEST_VECTOR_SIZE] = {0};

    fscanf(testvector_file, "%s", buffer);

   	bi_set_by_str(big_integer, buffer, POSITIVE, HEXADECIMAL);
}

/* write integer to file */
static void write_integer_into_file(FILE* file, bi* x)
{
    if (x->sign) 
		fprintf(file, "-");
    
    // write most significant position of big integer: %08x is not allowed.
    fprintf(file, "%x", x->a[x->dmax - 1]);

	// write big integer.
    for (int i = x->dmax - 2; i >= 0; i--)
        fprintf(file, "%08x", x->a[i]);

    fprintf(file, "\n");
}

/* test code */
void bi_test()
{
    FILE* f_operand_x = fopen(OPERAND_X_FILE_PATH, "r");
    FILE* f_operand_y = fopen(OPERAND_Y_FILE_PATH, "r");
    FILE* f_result = fopen(RESULT_FILE_PATH, "w");

    if(f_operand_x == NULL || f_operand_y == NULL) {
        perror("bi_test: file open error");
        exit(1);
    }

    bi* operand_x = NULL;
    bi* operand_y = NULL;
    bi* result = NULL;

    bi_new(&operand_x, 1);
    bi_new(&operand_y, 1);
    bi_new(&result, 1);

    uint32_t limit = 4225;

    /*-----------------------------------------------------------*/
	while(limit--)
	{
        /* 정수 읽어오기 */
    	read_integer_from_file(&operand_x, f_operand_x);
        read_integer_from_file(&operand_y, f_operand_y);

        /* 부호 바꾸기: 바꾸기 싫으면 주석처리 */
        //operand_x->sign = NEGATIVE;
       // operand_y->sign = NEGATIVE;

        /* 연산 */
    	bi_sub(&result, operand_x, operand_y);

        /* 연산 결과 저장 */
        write_integer_into_file(f_result, result);
	}
    /*-----------------------------------------------------------*/

    bi_delete(&operand_x);
    bi_delete(&operand_y);
	bi_delete(&result);

    fclose(f_operand_x);
    fclose(f_operand_y);
    fclose(f_result);
}

/* test code */
void bi_test_div()
{
    FILE* f_operand_x = fopen(OPERAND_X_FILE_PATH, "r");
    FILE* f_operand_y = fopen(OPERAND_Y_FILE_PATH, "r");
    FILE* f_result_q = fopen(RESULT_FILE_PATH_Q, "w");
    FILE* f_result_r = fopen(RESULT_FILE_PATH_R, "w");

    if(f_operand_x == NULL || f_operand_y == NULL) {
        perror("bi_test: file open error");
        exit(1);
    }

    bi* operand_x = NULL;
    bi* operand_y = NULL;
    bi* quotient = NULL;
    bi* remainder = NULL;

    bi_new(&operand_x, 1);
    bi_new(&operand_y, 1);
    bi_new(&quotient, 1);
    bi_new(&remainder, 1);

    /*-----------------------------------------------------------*/
	while(!feof(f_operand_x))
	{
        /* 정수 읽어오기 */
    	read_integer_from_file(&operand_x, f_operand_x);
        read_integer_from_file(&operand_y, f_operand_y);

        /* 부호 바꾸기: 바꾸기 싫으면 주석처리 */
        // operand_x->sign = NEGATIVE;
        // operand_y->sign = NEGATIVE;

        if(bi_is_zero(operand_y) == true) {
            fprintf(f_result_q, "DIV0!\n");
            fprintf(f_result_r, "DIV0!\n");
        } else {
            /* 연산 */
    	    bi_div_long(&quotient, &remainder, operand_x, operand_y);

            /* 연산 결과 저장 */
            write_integer_into_file(f_result_q, quotient);
            write_integer_into_file(f_result_r, remainder);
        }
        
	}
    /*-----------------------------------------------------------*/

    bi_delete(&operand_x);
    bi_delete(&operand_y);
	bi_delete(&quotient);
    bi_delete(&remainder);

    fclose(f_operand_x);
    fclose(f_operand_y);
    fclose(f_result_q);
    fclose(f_result_r);
}