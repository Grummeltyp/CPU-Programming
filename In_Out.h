/*
 * In_Out.h
 *
 *  Created on: 29.10.2013
 *      Author: Tom
 */

#ifndef IN_OUT_H_
#define IN_OUT_H_


typedef struct matrix /* a matrix */
{
	double* values; //pointer to the matrices entries
	int rows, columns; //Size of Matrix
} matrix;



double* alloc_matrix(int rows, int columns);


/* parses the specified line of the input file containing the matrices in textform
 * and writes up to 2 matrices to operandA and operandB.(If the line contains only 1 matrix, operandB is NULL)
 * Returns 1 if it succeeds, 0 otherwise.
 */

int parseMatrices(char* input, int line, matrix* operandA, matrix* operandB);

/*
 * prints the given matrix to specified output file. If output string is empty, prints to standard output.
 * returns 1 if succeeds, 0 otherwise
 */

int printMatrix(matrix* M, char* output);



#endif /* IN_OUT_H_ */
