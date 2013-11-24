//
//  Multiplication.c
//  Test2
//
//  Created by Tom on 19.11.13.
//  Copyright (c) 2013 Tom. All rights reserved.
//

#include <stdio.h>
#include <time.h>

#include <omp.h>


#include "In_Out.h"
#include "Multiplication.h"

int matchDimensions(matrix* A, matrix* B, matrix* result)
{
    if (A->columns != B->rows)
    {
        perror("Matrices dont match for multiplication!");
        return 0;
    }
    result->rows = A->rows;
    result->columns = B->columns;
    result->values = alloc_matrix(result->rows, result->columns);
    return 1;
}


/*
 *compares to matrices A and B. Returns 1 if they are equal, returns 0 otherwise
 */

int compareMatrices(matrix* A, matrix* B)
{
    if (A->rows != B->rows || A->columns != B->columns || sizeof(A->values) != sizeof(B->values)) return 0;
    
    for (int i = 0; i<A->rows*A->columns; ++i)
    {
        if (A->values[i] != B->values[i]) return 0;
    }
    return 1;
}


/*
 * returns the difference between two timespec structs, allowing for performance measurements of a function
 * Code taken from: http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 */
struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int sequential(matrix* A, matrix* B, matrix* result)
{
    if (!matchDimensions(A, B, result)) return 0;
    
    for (int i = 0; i<A->rows; ++i) {
        
        for (int j = 0; j<B->columns; ++j) {
            double element = 0;
            
            for (int k = 0; k<A->columns; ++k) {
                element += A->values[i * A->columns + k] * B->values[k * B->columns + j];
            }
            result->values[i * result->columns + j] = element;
        }
    }
    return 1;
}

int openMP(matrix* A, matrix* B, matrix* result)
{
    if (!matchDimensions(A, B, result)) return 0;

    #pragma omp parallel
    {
        int num = omp_get_num_threads();
        printf("%d Threads\n", num);
        #pragma omp for
        for (int i = 0; i<A->rows; ++i) {
            
            for (int j = 0; j<B->columns; ++j) {
                double element = 0;
                
                for (int k = 0; k<A->columns; ++k) {
                    element += A->values[i * A->columns + k] * B->values[k * B->columns + j];
                }
                result->values[i * result->columns + j] = element;
            }
        }
    }
    return 1;
}


int main(int argc, const char * argv[])
{
    struct timespec start, end, difference;
    matrix M, N, seq, par;
    parseMatrices("matrices.txt", 0, &M, &N);
   // clock_gettime(CLOCK_REALTIME, &start);
    sequential(&M, &N, &seq);
    //clock_gettime(CLOCK_REALTIME, &end);
    //difference = diff(start, end);
    openMP(&M, &N, &par);
    printMatrix(&seq, "");
    printMatrix(&par, "");

    
    return 0;
}

