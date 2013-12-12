#ifndef IN_OUT_H_
#define IN_OUT_H_

#include "matrix.h"

/* parses the specified line of the input file containing the matrices in
 * textform
 * and writes up to 2 matrices to operandA and operandB.(If the line contains
 * only 1 matrix, operandB is NULL)
 * Returns 1 if it succeeds, 0 otherwise.
 */

int parseMatrices(char* input, int line, matrix* operandA, matrix* operandB);

/*
 * prints the given matrix to specified output file. If output string is empty,
 * prints to standard output.
 * returns 1 if succeeds, 0 otherwise
 */

int printMatrix(matrix* M, char* output);



#endif /* IN_OUT_H_ */
