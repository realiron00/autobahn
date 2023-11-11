#include <stdio.h>
#include <stdlib.h>
#include </opt/homebrew/Cellar/gmp/6.2.1_1/include/gmp.h>  //! 이거 <gmp.h>로 어떻게 못하나요?

#define OPERAND_X_FILEPATH "tv_x.txt"
#define OPERAND_Y_FILEPATH "tv_y.txt"

#define RESULT_ADD_FILEPATH_PP "tv_add_pp.txt"
#define RESULT_ADD_FILEPATH_PN "tv_add_pn.txt"
#define RESULT_ADD_FILEPATH_NP "tv_add_np.txt"
#define RESULT_ADD_FILEPATH_NN "tv_add_nn.txt"

#define RESULT_SUB_FILEPATH_PP "tv_sub_pp.txt"
#define RESULT_SUB_FILEPATH_PN "tv_sub_pn.txt"
#define RESULT_SUB_FILEPATH_NP "tv_sub_np.txt"
#define RESULT_SUB_FILEPATH_NN "tv_sub_nn.txt"

#define RESULT_MUL_FILEPATH_PP "tv_mul_pp.txt"
#define RESULT_MUL_FILEPATH_PN "tv_mul_pn.txt"
#define RESULT_MUL_FILEPATH_NP "tv_mul_np.txt"
#define RESULT_MUL_FILEPATH_NN "tv_mul_nn.txt"

#define RESULT_DIV_Q_FILEPATH_PP "tv_div_q_pp.txt"
#define RESULT_DIV_Q_FILEPATH_PN "tv_div_q_pn.txt"
#define RESULT_DIV_Q_FILEPATH_NP "tv_div_q_np.txt"
#define RESULT_DIV_Q_FILEPATH_NN "tv_div_q_nn.txt"

#define RESULT_DIV_R_FILEPATH_PP "tv_div_r_pp.txt"
#define RESULT_DIV_R_FILEPATH_PN "tv_div_r_pn.txt"
#define RESULT_DIV_R_FILEPATH_NP "tv_div_r_np.txt"
#define RESULT_DIV_R_FILEPATH_NN "tv_div_r_nn.txt"

#define TEST_VECTOR_SIZE 512
#define HEXADECIMAL 16

void generate_testvectors_input(uint32_t bitlen);
void generate_test_vectors_addition();