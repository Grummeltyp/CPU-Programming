#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct matrix /* a matrix */
{
  double* values; //pointer to the matrices entries
  int rows, columns; //Size of Matrix
} matrix;

double* alloc_matrix(int rows, int columns);

int matchDimensions(matrix* A, matrix* B, matrix* result);


#endif