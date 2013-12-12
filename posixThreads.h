#ifndef POSIXTHREADS_H_
#define POSIXTHREADS_H_

#include <pthread.h>
#include <stdlib.h>

#include "matrix.h"

typedef struct thread_args //stores the arguments which will be given to each thread
{
    matrix* First;
    matrix* Second;
    matrix* Res;
    int row;
    int column;
} thread_args;

/**
* Computation of Scalar Product for Pthread Implementation
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
    // printf("Element: %i %i, %lf\n", arguments->column, arguments->row,res);
    arguments->Res->values[(arguments->row * arguments->Res->columns) + arguments->column] = res;
}

int multithreaded(matrix* A, matrix* B, matrix* result)
{
   if (!matchDimensions(A, B, result)) return 0;

    pthread_t* identifiers = malloc(sizeof(pthread_t) * result->rows * result->columns);

    thread_args* arguments = malloc(sizeof(thread_args) * result->rows * result->columns);

    for (int i = 0; i < result->rows; ++i)
    {
        for (int j = 0; j < result->columns; ++j)
        {
            int run = i * result->columns + j;
            // arguments[run] = {A,B,result,i,j};
            arguments[run].First = A;
            arguments[run].Second = B;
            arguments[run].Res = result;
            arguments[run].row = i;
            arguments[run].column = j;

            pthread_create(&identifiers[run], NULL, scalarProduct, &arguments[run]);
          //  printf("Waiting for thread %d", i*result->columns + j);
            // pthread_join(identifiers[(i*(result->columns)) + j], NULL);
        }
    }

    for (int k = 0; k < result->rows * result->columns; ++k) //waiting for the threads to finish
    {
        pthread_join(identifiers[k], NULL);
    }

    free(identifiers);

    return 1;
}

#endif
