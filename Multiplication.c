//
//  Multiplication.c
//  Test2
//
//  Created by Tom on 19.11.13.
//  Copyright (c) 2013 Tom. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <omp.h>
#include <pthread.h>


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
      //  int num = omp_get_num_threads();
       // printf("%d Threads\n", num);
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

typedef struct thread_args //stores the arguments which will be given to each thread
{
    matrix* First;
    matrix* Second;
    matrix* Res;
    int row;
    int column;
} thread_args;

/**
* Returns the scalar product of a row and a column of to matrices
*
* @param *A handle to the first matrix
* @param *B handle to the second matrix
* @param row indicates which row of the first matrix will be used 
* @param column indicates which column of the second matrix will be used 
* @param[out] *result handle to the result matrix 
*/
void* scalarProduct(thread_args* arguments)
{
    double res = 0;
    for (int i = 0; i < arguments->First->columns; ++i)
    {
        res += arguments->First->values[(arguments->row * arguments->First->columns) + i] * arguments->Second->values[i * arguments->Second->columns + arguments->column];
    }
    arguments->Res->values[(arguments->row * arguments->Res->columns) + arguments->column] = res;
}

int multithreaded(matrix* A, matrix* B, matrix* result)
{
   if (!matchDimensions(A, B, result)) return 0;

    thread_args arguments;
    arguments.First = A;
    arguments.Second = B;
    arguments.Res = result;

    pthread_t* identifiers = malloc(sizeof(pthread_t) * result->rows * result->columns);
    for (int i = 0; i < result->rows; ++i)
    {
        arguments.row = i;
        for (int j = 0; j < result->columns; ++j)
        {
            arguments.column = j;
            pthread_create(&identifiers[(i*(result->columns)) + j], NULL, scalarProduct, &arguments);
          //  printf("Waiting for thread %d", i*result->columns + j);
            pthread_join(identifiers[(i*(result->columns)) + j], NULL);
        }
    }


    free(identifiers);

    return 1;
}



int main(int argc, const char * argv[])
{
    struct timeval start, end;
    matrix M, N, seq, res;
    parseMatrices("matrices.txt", 0, &M, &N);
    printMatrix(&M,"");
    printf("\n");
    printMatrix(&N,"");
    printf("\n");
   // clock_gettime(CLOCK_REALTIME, &start);
    gettimeofday(&start, NULL);
    sequential(&M, &N, &res);
    gettimeofday(&end, NULL);
    
    printMatrix(&res, "");
    printf("Seq-Implementation took %.3lf milliseconds.\n", (end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    //clock_gettime(CLOCK_REALTIME, &end);
    //difference = diff(start, end);
    gettimeofday(&start, NULL);
    openMP(&M, &N, &res);
    gettimeofday(&end, NULL);
    
    printMatrix(&res, "");
    printf("Openmp-Implementation took %.3lf milliseconds.\n", (end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    gettimeofday(&start, NULL);
    multithreaded(&M, &N, &res);
    gettimeofday(&end, NULL);

    printMatrix(&res, "");
    printf("Pthreads-Implementation took %.3lf milliseconds.\n", (end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));
   
    free(M.values);
    free(N.values);
    free(res.values);



    
    return 0;
}

