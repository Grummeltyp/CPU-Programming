#ifndef IN_OUT_H_
#define IN_OUT_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define EPSILON 0.000001

#include "matrix.h"

double getDifference(struct timeval start, struct timeval end);

/*
 *compares two matrices A and B. Returns 1 if they are equal, returns 0 otherwise
 */

int compareMatrices(matrix* A, matrix* B);

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
