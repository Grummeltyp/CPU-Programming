#ifndef POSIXTHREADS_H_
#define POSIXTHREADS_H_

#include <pthread.h>
#include <stdlib.h>

#include "matrix.h"

typedef struct thread_args //stores the arguments which will be given to each thread
{
    matrix* A;
    matrix* B;
    matrix* Res;
    int first; //first element of result matrix the thread has to compute
    int last; //last element of result matrix the thread has to compute
} thread_args;

/**
* multiplies a thread portion of the 2 matrices
*
* @param *A handle to the first matrix
* @param *B handle to the second matrix
* @param first first element of result matrix the thread shall compute
* @param last last element of result matrix the thread shall compute
* @param[out] *Res handle to the result matrix
*/
void* partMultiplication(thread_args* arguments)
{
  for (int elem = arguments->first; elem <= arguments->last; ++elem)
  {

    double res = 0;
    for (int i = 0; i < arguments->A->columns; ++i)
    {
      res += arguments->A->values[(elem/arguments->Res->columns)
                * arguments->A->columns + i]
             * arguments->B->values[i * arguments->B->columns
                + (elem % arguments->Res->columns)];
    }
    arguments->Res->values[elem] = res;

  }
}

int multithreaded(matrix* A, matrix* B, matrix* result, int number_of_threads)
{
   if (!matchDimensions(A, B, result)) return 0;

    pthread_t* identifiers = malloc(sizeof(pthread_t) * number_of_threads);

    thread_args* arguments = malloc(sizeof(thread_args) * number_of_threads);

    int part = (result->rows * result->columns)/number_of_threads;
    int low = 0;
    for (int i = 0; i < number_of_threads; ++i)
    {
      arguments[i].A = A;
      arguments[i].B = B;
      arguments[i].Res = result;
      arguments[i].first = low;
      //if elements cant be equally distributed among threads, last thread gets
      //all remaining elements
      if ((result->rows * result->columns) % number_of_threads != 0 &&
        i == number_of_threads - 1)
      {
        arguments[i].last = (result->rows * result->columns) - 1;
      }
      else arguments[i].last = low + part - 1;
      low = low + part;

      pthread_create(&identifiers[i], NULL, partMultiplication
        , &arguments[i]);
    }

    // for (int i = 0; i < result->rows; ++i)
    // {
    //     for (int j = 0; j < result->columns; ++j)
    //     {
    //         int run = i * result->columns + j;
    //         // arguments[run] = {A,B,result,i,j};
    //         arguments[run].First = A;
    //         arguments[run].Second = B;
    //         arguments[run].Res = result;
    //         arguments[run].row = i;
    //         arguments[run].column = j;

    //         pthread_create(&identifiers[run], NULL, partMultiplication
    //           , &arguments[run]);
    //       //  printf("Waiting for thread %d", i*result->columns + j);
    //         // pthread_join(identifiers[(i*(result->columns)) + j], NULL);
    //     }
    // }

    for (int k = 0; k < number_of_threads; ++k) //waiting for the threads to end
    {
        pthread_join(identifiers[k], NULL);
    }

    free(identifiers);

    return 1;
}

#endif
