#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "In_Out.h"
#include "matrix.h"

#include "posixThreads.h"
#include "messagePassing.h"
#include "openMP.h"



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

int main(int argc, char * argv[])
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
    printf("Seq-Implementation took %.3lf milliseconds.\n", (end.tv_sec
      - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    //clock_gettime(CLOCK_REALTIME, &end);
    //difference = diff(start, end);
    gettimeofday(&start, NULL);
    openMP(&M, &N, &res);
    gettimeofday(&end, NULL);

    printMatrix(&res, "");
    printf("Openmp-Implementation took %.3lf milliseconds.\n", (end.tv_sec
      - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    gettimeofday(&start, NULL);
    multithreaded(&M, &N, &res);
    gettimeofday(&end, NULL);

    printMatrix(&res, "");
    printf("Pthreads-Implementation took %.3lf milliseconds.\n", (end.tv_sec
      - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    gettimeofday(&start, NULL);
    messagePassing(&M, &N, &res, argc, argv);
    gettimeofday(&end, NULL);

    printMatrix(&res, "");
    printf("Messagepassing-Implementation took %.3lf milliseconds.\n",
      (end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    free(M.values);
    free(N.values);
    free(res.values);




    return 0;
}