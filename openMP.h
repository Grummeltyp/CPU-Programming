#ifndef OPENMP_H_
#define OPENMP_H_

#include <omp.h>
#include <stdlib.h>

#include "matrix.h"


int openMP(matrix* A, matrix* B, matrix* result, int number_of_threads)
{
    if (!matchDimensions(A, B, result)) return 0;

    omp_set_num_threads(number_of_threads);

    #pragma omp parallel
    {
      //  int num = omp_get_num_threads();
       // printf("%d Threads\n", num);
      for (int i = 0; i<A->rows; ++i)
      {

        #pragma omp for
        for (int j = 0; j<B->columns; ++j)
        {
          double element = 0;

          for (int k = 0; k<A->columns; ++k)
          {
            element += A->values[i * A->columns + k] * B->values[k
              * B->columns + j];
          }
          result->values[i * result->columns + j] = element;
        }
      }
    }
    return 1;
}

#endif