#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.h"
//#include "messagePassing.h"
#include "In_Out.h"

#define PATH_TO_TESTS "matrices.txt"
#define PATH_TO_RESULTS "results.txt"
#define PATH_TO_TIMES "performance.txt"
#define MAX_TEST_CASES 10

matrix firsts[MAX_TEST_CASES];
matrix seconds[MAX_TEST_CASES];
matrix results[MAX_TEST_CASES];
int rank; //rank of current task
int num; //number of tasks
int low, up; //boundaries of portions to compute by each task
int part;
MPI_Status status; // store statusus of MPI_Recv
MPI_Request req; //capture request of MPI_Isend
struct timeval start, end;


int main(int argc, char* argv[])
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num);

  if(num == 1)
  {
    printf("MessagePassing with one job only doesn't make sense.\n");
    exit(0);
  }

  if(rank == 0) //initial work by master
  {
    printf("Reading matrices from InputFile...\n");
    for (int i = 0; i < MAX_TEST_CASES; ++i)
    {
      parseMatrices(PATH_TO_TESTS, i, &firsts[i], &seconds[i]);
    }
    printf("Calculating testcases...\n");
    gettimeofday(&start, NULL);
  }

  for (int cur = 0; cur < MAX_TEST_CASES; ++cur)
  {

    if(rank == 0)
    {
      if (!matchDimensions(&firsts[cur], &seconds[cur]
        , &results[cur])) exit(-1);

      part = firsts[cur].rows / (num - 1); //how many rows for each worker?

      for (int i = 1; i < num; ++i)
      {
        //send dimensions of allocated matrices to workers for them to
        //allocate
        MPI_Isend(&(firsts[cur].rows), 1, MPI_INT, i, 7+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&(firsts[cur].columns), 1, MPI_INT, i, 8+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&(seconds[cur].rows), 1, MPI_INT, i, 9+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&(seconds[cur].columns), 1, MPI_INT, i, 10+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&(results[cur].rows), 1, MPI_INT, i, 11+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&(results[cur].columns), 1, MPI_INT, i, 12+cur, MPI_COMM_WORLD, &req);

        low = part * (i - 1);

        //if A's rows are not dividable by the number of workers, last worker
        //gets all remaining rows
        if (i == num - 1 && firsts[cur].rows % (num - 1) != 0) up = firsts[cur].rows;
        else up = low + part;

        // printf("Master Sending...\n");
        //non-blocking sends of boundaries to current worker
        MPI_Isend(&low, 1, MPI_INT, i, 1+cur, MPI_COMM_WORLD, &req);
        MPI_Isend(&up, 1, MPI_INT, i, 2+cur, MPI_COMM_WORLD, &req);

        //non-blocking send of portion of matrix A to current worker
        MPI_Isend(&(firsts[cur].values[low * firsts[cur].columns]), (up - low)
          * firsts[cur].columns, MPI_DOUBLE, i, 3+cur, MPI_COMM_WORLD, &req);
      }
        // printf("Master done Sending.\n");
    }


    if(rank > 0)
    {
      MPI_Recv(&(firsts[cur].rows), 1, MPI_INT, 0, 7+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&(firsts[cur].columns), 1, MPI_INT, 0, 8+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&(seconds[cur].rows), 1, MPI_INT, 0, 9+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&(seconds[cur].columns), 1, MPI_INT, 0, 10+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&(results[cur].rows), 1, MPI_INT, 0, 11+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&(results[cur].columns), 1, MPI_INT, 0, 12+cur, MPI_COMM_WORLD, &status);

      //allocate matrices in worker's memory
      firsts[cur].values = alloc_matrix(firsts[cur].rows, firsts[cur].columns);
      seconds[cur].values = alloc_matrix(seconds[cur].rows, seconds[cur].columns);

      //result matrix gets calloced, because its elements need to be 0 at start
      results[cur].values = calloc(sizeof(double),
        results[cur].rows * results[cur].columns);
    }

    MPI_Bcast(&(seconds[cur].values[0]), seconds[cur].rows * seconds[cur].columns
      , MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank > 0) //worker tasks
    {

      //recieving boundaries
      // printf("Slave recieving...\n");
      MPI_Recv(&low, 1, MPI_INT, 0, 1+cur, MPI_COMM_WORLD, &status);
      MPI_Recv(&up, 1, MPI_INT, 0, 2+cur, MPI_COMM_WORLD, &status);

      //recieving portion of A
      MPI_Recv(&(firsts[cur].values[low * firsts[cur].columns])
        , (up - low) * firsts[cur].columns, MPI_DOUBLE
        , 0, 3+cur, MPI_COMM_WORLD, &status);

    // printf("Slave done recieving.\n");

      for (int i = low; i < up; ++i)
      {
        for (int j = 0; j < seconds[cur].columns; ++j)
        {
          for (int k = 0; k < seconds[cur].rows; ++k)
          {
            results[cur].values[i * results[cur].columns + j] +=
              firsts[cur].values[i * firsts[cur].columns + k]
              * seconds[cur].values[k * seconds[cur].columns + j];
          }
        }
      }

      // printf("Slave sending...\n");
      MPI_Isend(&low, 1, MPI_INT, 0, 4+cur, MPI_COMM_WORLD, &req);
      MPI_Isend(&up, 1, MPI_INT, 0, 5+cur, MPI_COMM_WORLD, &req);
      MPI_Isend(&(results[cur].values[low * results[cur].columns])
        , (up - low) * results[cur].columns
        , MPI_DOUBLE, 0, 6+cur, MPI_COMM_WORLD, &req);
      // printf("Slave done sending.\n");

    }

    if (rank == 0)
    {
     // printf("Master recieving...\n");
      for (int i = 1; i < num; ++i)
      {
        MPI_Recv(&low, 1, MPI_INT, i, 4+cur, MPI_COMM_WORLD, &status);
        MPI_Recv(&up, 1, MPI_INT, i, 5+cur, MPI_COMM_WORLD, &status);

        MPI_Recv(&(results[cur].values[low * results[cur].columns])
          , (up - low) * seconds[cur].columns
          , MPI_DOUBLE, i, 6+cur, MPI_COMM_WORLD, &status);
      }
      // printf("Master done recieving.\n");
    }
  }

  if (rank == 0) //end work by master
  {
    gettimeofday(&end, NULL);

    matrix originalResults[MAX_TEST_CASES];
    printf("Reading results from sequential Implementation for comparision...\n");

    for (int i = 0; i < MAX_TEST_CASES; ++i)
    {
      parseMatrices(PATH_TO_RESULTS, i, &originalResults[i], NULL);
    }

    for (int i = 0; i < MAX_TEST_CASES; ++i)
    {
      if(!compareMatrices(&results[i], &originalResults[i]))
      {
        printf("MessagePassing-Implementation has faults!");
        MPI_Finalize();
        exit(0);
      }
    }
    FILE* performance;
    performance = fopen(PATH_TO_TIMES, "a");
    fprintf(performance,"MessagePassing-Implementation took %lf seconds for all testcases.\n"
      , getDifference(start, end));

    fclose(performance);
    for (int i = 0; i < MAX_TEST_CASES; ++i)
    {
      free(firsts[i].values);
      free(seconds[i].values);
      free(results[i].values);
      free(originalResults[i].values);
    }
  }
  //   gettimeofday(&end, NULL);

  //   printMatrix(&result, "");
  //   printf("MPI-Implementation took %.3lf milliseconds.\n", (end.tv_sec
  //   - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec)*1000));


  // free(A.values);
  // free(B.values);
  // free(result.values);
  // }

  MPI_Finalize();
  exit(0);
}