/*
 * Multiplication.h
 *
 *  Created on: 29.10.2013
 *      Author: Tom
 */

#ifndef MULTIPLICATION_H_
#define MULTIPLICATION_H_

#include "In_Out.h"

/*
 * The different implementations of Matrix Multiplication
 */

int sequential(matrix* A, matrix* B, matrix* result);

int openMP(matrix* A, matrix* B, matrix* result);

int multithreaded(matrix* A, matrix* B, matrix* result);

int messagePassing(matrix* A, matrix* B, matrix* result);



#endif /* MULTIPLICATION_H_ */
