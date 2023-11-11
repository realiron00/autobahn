#include "gen_tv.h"

// gcc *.c -L/opt/homebrew/Cellar/gmp/6.2.1_1/lib -lgmp
//! 음수가 섞인 테스트벡터도 만들어야함.

/**
 * @brief 연산 테스트를 위한 피연산자 테스트벡터 생성 함수
 * 
 * 정확한 검증을 위해 비트 단위로 피연산자를 생성함.
 * ex. 3bit 테스트벡터 생성 --> 0b000, 0b001, 0b011, 0b111 생성
 * 
 * @param bitlen input, 피연산자의 비트 길이
 */
void generate_operand_testvectors(uint32_t bitlen)
{
    FILE* f_operand_x = fopen(OPERAND_X_FILEPATH, "w");
    FILE* f_operand_y = fopen(OPERAND_Y_FILEPATH, "w");

    if (f_operand_x == NULL || f_operand_y == NULL) {
        perror("Error opening file");
        exit(1);
    }

    mpz_t operand_x;
    mpz_t operand_y;  

    mpz_init(operand_x);
    mpz_init(operand_y);     

    mpz_set_str(operand_x, "0", 10);
    mpz_set_str(operand_y, "0", 10); 

    // generate positive test vector files
    for(size_t i = 0; i < bitlen+1; i++)
    {
        // y: set the integer to 0
        mpz_set_str(operand_y, "0", 10);

        for(size_t j = 0; j < bitlen+1; j++)
        {
            // Write the result in hexadecimal format to the file
            gmp_fprintf(f_operand_x, "%Zx\n", operand_x);
            gmp_fprintf(f_operand_y, "%Zx\n", operand_y);

            // y: left shift a bit and set the LSB to 1
            mpz_mul_2exp(operand_y, operand_y, 1);
            mpz_setbit(operand_y, 0);
        }

        // x: left shift a bit and set the LSB to 1
        mpz_mul_2exp(operand_x, operand_x, 1);
        mpz_setbit(operand_x, 0);
    }

    mpz_clear(operand_x);
    mpz_clear(operand_y);

    fclose(f_operand_x);
    fclose(f_operand_y);
}

/**
 * @brief 특정 연산에 대한 테스트벡터 생성 함수
 * 
 */
void generate_testvectors()
{
    FILE* f_operand_x = fopen(OPERAND_X_FILEPATH, "r");
    FILE* f_operand_y = fopen(OPERAND_Y_FILEPATH, "r");

    FILE* f_result_pp = fopen(RESULT_MUL_FILEPATH_PP, "w");
    FILE* f_result_pn = fopen(RESULT_MUL_FILEPATH_PN, "w");
    FILE* f_result_np = fopen(RESULT_MUL_FILEPATH_NP, "w");
    FILE* f_result_nn = fopen(RESULT_MUL_FILEPATH_NN, "w");

    if (f_operand_x == NULL || f_operand_y == NULL) {
        perror("Error opening file");
        exit(1);
    }

    mpz_t operand_x_p;
    mpz_t operand_x_n;
    mpz_t operand_y_p;
    mpz_t operand_y_n;
    mpz_t result_pp;
    mpz_t result_pn;
    mpz_t result_np;
    mpz_t result_nn;

    mpz_init(operand_x_p);
    mpz_init(operand_x_n);
    mpz_init(operand_y_p);
    mpz_init(operand_y_n);
    mpz_init(result_pp);
    mpz_init(result_pn);
    mpz_init(result_np);
    mpz_init(result_nn);

    // generate test vectors for the operation.
    while(!feof(f_operand_x))
    {   
        // get integer
        gmp_fscanf(f_operand_x, "%Zx", operand_x_p);
        gmp_fscanf(f_operand_y, "%Zx", operand_y_p);
        mpz_neg(operand_x_n, operand_x_p);
        mpz_neg(operand_y_n, operand_y_p);

        // opraiting
        mpz_mul(result_pp, operand_x_p, operand_y_p);
        mpz_mul(result_pn, operand_x_p, operand_y_n);
        mpz_mul(result_np, operand_x_n, operand_y_p);
        mpz_mul(result_nn, operand_x_n, operand_y_n);

        // write
        gmp_fprintf(f_result_pp, "%Zx\n", result_pp);
        gmp_fprintf(f_result_pn, "%Zx\n", result_pn);
        gmp_fprintf(f_result_np, "%Zx\n", result_np);
        gmp_fprintf(f_result_nn, "%Zx\n", result_nn);
    }

    mpz_clear(operand_x_p);
    mpz_clear(operand_x_n);
    mpz_clear(operand_y_p);
    mpz_clear(operand_y_n);
    mpz_clear(result_pp);
    mpz_clear(result_pn);
    mpz_clear(result_np);
    mpz_clear(result_nn);

    fclose(f_operand_x);
    fclose(f_operand_y);
    fclose(f_result_pp);
    fclose(f_result_pn);
    fclose(f_result_np);
    fclose(f_result_nn);
}

int main()
{
    generate_testvectors();

    return 0;
}

/*

        if(mpz_cmp_ui(operand_y_p, 0) != 0) {
            mpz_tdiv_r(result_pp, operand_x_p, operand_y_p);
            mpz_tdiv_r(result_np, operand_x_n, operand_y_p);
            gmp_fprintf(f_result_pp, "%Zx\n", result_pp);
            gmp_fprintf(f_result_np, "%Zx\n", result_np);
        } else {
            fprintf(f_result_pp, "DIV0!\n");
            fprintf(f_result_np, "DIV0!\n");
        }

        if(mpz_cmp_ui(operand_y_n, 0) != 0) {
            mpz_tdiv_r(result_pn, operand_x_p, operand_y_n);
            mpz_tdiv_r(result_nn, operand_x_n, operand_y_n);
            gmp_fprintf(f_result_pn, "%Zx\n", result_pn);
            gmp_fprintf(f_result_nn, "%Zx\n", result_nn);
        } else {
            fprintf(f_result_pn, "DIV0!\n");
            fprintf(f_result_nn, "DIV0!\n");
        }

*/