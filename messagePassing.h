#ifndef MESSAGEPASSING_H_
#define MESSAGEPASSING_H_

#include <mpi.h>
#include <stdlib.h>

#include "matrix.h"


int rank; //rank of current task
int num; //number of tasks
int low, up; //boundaries of portions to compute by each task
int part;
MPI_Status stat; // store status of MPI_Recv
MPI_Request req; //capture request of MPI_Isend


int messagePassing(matrix* A, matrix* B, matrix* result)
{

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

      printf("Master Sending...\n");
      //non-blocking sends of boundaries to current worker
      MPI_Isend(&low, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &req);
      MPI_Isend(&up, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &req);

      //non-blocking send of portion of matrix A to current worker
      MPI_Isend(&(A->values[low * A->columns]), (up - low) * A->columns
        , MPI_DOUBLE, i, 3, MPI_COMM_WORLD, &req);
    }
    printf("Master done Sending.\n");

  }

  //all workers get the whole of B
  MPI_Bcast(&(B->values), B->rows * B->columns, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank > 0) //worker tasks
  {
    //recieving boundaries
    printf("Slave recieving...\n");
    MPI_Recv(&low, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
    MPI_Recv(&up, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &stat);

    //recieving portion of A
    MPI_Recv(&(A->values[low * A->columns]), (up - low) * A->columns, MPI_DOUBLE
      , 0, 3, MPI_COMM_WORLD, &stat);
    printf("Slave done recieving.\n");


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

    printf("Slave sending...\n");
    MPI_Isend(&low, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &req);
    MPI_Isend(&up, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &req);
    MPI_Isend(&(result->values[low * result->columns]), (up - low) * B->columns
      , MPI_DOUBLE, 0, 6, MPI_COMM_WORLD, &req);
    printf("Slave done sending.\n");

  }

  if (rank == 0)
  {
    for (int i = 1; i < num; ++i)
    {
      printf("Master recieving...\n");
      MPI_Recv(&low, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &stat);
      MPI_Recv(&up, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &stat);

      MPI_Recv(&(result->values[low * result->columns]), (up - low) * B->columns
        , MPI_DOUBLE, i, 6, MPI_COMM_WORLD, &stat);
    }
    printf("Master done recieving.\n");

  }

  return 1;
}

#endif
