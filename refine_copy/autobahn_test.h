#ifndef AUTOBAHN_TEST_H
#define AUTOBAHN_TEST_H

#include "autobahn.h"

#define OPERAND_X_FILE_PATH "test/tv_x.txt"
#define OPERAND_Y_FILE_PATH "test/tv_y.txt"
#define RESULT_FILE_PATH "test/_tv_result.txt"

#define RESULT_FILE_PATH_Q "test/_tv_result_q.txt"
#define RESULT_FILE_PATH_R "test/_tv_result_r.txt"

#define TEST_VECTOR_SIZE 512

void bigint_test();
void bigint_test_div();

#endif