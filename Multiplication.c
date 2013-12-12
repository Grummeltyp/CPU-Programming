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
#include <mpi.h>


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

int rank; //rank of current task
int num; //number of tasks
int low, up; //boundaries of portions to compute by each task
int part;
MPI_Status stat; // store status of MPI_Recv
MPI_Request req; //capture request of MPI_Isend


int messagePassing(matrix* A, matrix* B, matrix* result, int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num);

  if (rank == 0) //master task
  {
    if (!matchDimensions(A, B, result)) return 0;

    part = A->rows / (num - 1); //how many rows for each worker?

    for (int i = 1; i < num; ++i)
    {
      low = part * (i - 1);

      //if A's rows are not dividable by the number of workers, last worker
      //gets all remaining rows
      if (i == num - 1 && A->rows % (num - 1) != 0) up = A->rows;
      else up = low + part;

      //non-blocking sends of boundaries to current worker
      MPI_Isend(&low, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &req);
      MPI_Isend(&up, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &req);

      //non-blocking send of portion of matrix A to current worker
      MPI_Isend(&(A->values[low * A->columns]), (up - low) * A->columns
        , MPI_DOUBLE, i, 3, MPI_COMM_WORLD, &req);
    }
    //all workers get the whole of B
    MPI_Bcast(&(B->values), B->rows * B->columns, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 1; i < num; ++i)
    {
      MPI_Recv(&low, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &stat);
      MPI_Recv(&up, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &stat);

      MPI_Recv(&(result->values[low * result->columns]), (up - low) * B->columns
        , MPI_DOUBLE, i, 6, MPI_COMM_WORLD, &stat);
    }
  }

  if (rank > 0) //worker tasks
  {
    //recieving boundaries
    MPI_Recv(&low, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
    MPI_Recv(&up, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &stat);

    //recieving portion of A
    MPI_Recv(&(A->values[low * A->columns]), (up - low) * A->columns, MPI_DOUBLE
      , 0, 3, MPI_COMM_WORLD, &stat);

    for (int i = low; i < up; ++i)
    {
      for (int j = 0; j < B->columns; ++j)
      {
        for (int k = 0; i < B->rows; ++k)
        {
          result->values[i * result->columns + j] +=
            A->values[i * A->columns + k] * B->values[k * B->columns + j];
        }
      }
    }
  }

  MPI_Finalize();
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
    messagePassing(&M, &N, &res);
    gettimeofday(&end, NULL);

    printMatrix(&res, "");
    printf("Messagepassing-Implementation took %.3lf milliseconds.\n",
      (end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));

    free(M.values);
    free(N.values);
    free(res.values);




    return 0;
}

