#include <stdio.h>
#include <stdlib.h>

#include "In_Out.h"
#include "matrix.h"

#include "posixThreads.h"
#include "openMP.h"

#define PATH_TO_TESTS "matrices.txt"
#define PATH_TO_RESULTS "results.txt"
#define PATH_TO_TIMES "performance.txt"
#define MAX_TEST_CASES 10

int threads = 2;


int sequential(matrix* A, matrix* B, matrix* result)
{
    if (!matchDimensions(A, B, result)) return 0;

    for (int i = 0; i<A->rows; ++i) {

        for (int j = 0; j<B->columns; ++j) {
            double element = 0;

            for (int k = 0; k<A->columns; ++k) {
                element += A->values[i * A->columns + k]
                  * B->values[k * B->columns + j];
            }
            result->values[i * result->columns + j] = element;
        }
    }
    return 1;
}

int main(int argc, char * argv[])
{

  //checking for desired number of threads
  if (argc != 1)
  {
    if (strcmp(argv[1],"-t") || (threads = atoi(argv[2])) < 2)
    {
      printf("Usage: %s [-t <NUMBER_OF_THREADS>]\n Default Number of Threads: 2\n"
        , argv[0]);
      exit(-1);
    }
    printf("Pthreads and OpenMP Calculations will be done with %i threads.\n", threads);
  }
  struct timeval start, end;
  matrix firsts[MAX_TEST_CASES];
  matrix seconds[MAX_TEST_CASES];
  matrix results[MAX_TEST_CASES];


  //parses all Testmatrices and stores them in two arrays
  // int num; //the actual number of testcases
  printf("Reading matrices from InputFile...\n");
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
    int end = parseMatrices(PATH_TO_TESTS, i, &firsts[i], &seconds[i]);
    if (end == 0)
    {
      printf("Parsing went wrong\n");
      exit(0);
    }
  }

  FILE* performance;

  //first, sequential computation
  printf("Sequential calculation...\n");
  gettimeofday(&start, NULL);
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
    sequential(&firsts[i], &seconds[i], &results[i]);
  }
  gettimeofday(&end, NULL);

  //prints the results of sequential computation to file
  printf("Printing results...\n");
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
    if(!printMatrix(&results[i], PATH_TO_RESULTS))
    {
      printf("Theres a problem with the output stream.\n");
      exit(0);
    }
  }
  printf("Sequential calculation complete. Check %s for results\n"
    , PATH_TO_RESULTS);

  performance = fopen(PATH_TO_TIMES, "w");
  fprintf(performance,
    "Sequential Implementation took %.3lf seconds for all testcases.\n"
    , getDifference(start, end));

  //openMP Implementation is up next
  matrix ompresults[MAX_TEST_CASES];
  printf("OpenMP calculation...\n");
  gettimeofday(&start, NULL);
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
    openMP(&(firsts[i]), &(seconds[i]), &(ompresults[i]), threads);
  }
  gettimeofday(&end, NULL);

  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
      if(!compareMatrices(&results[i], &ompresults[i]))
      {
        printf("OMP-Implementation has faults!");
        exit(0);
      }
  }

  fprintf(performance,
    "OpenMP-Implementation took %.3lf seconds for all testcases.\n"
    , getDifference(start, end));

  //lastly, pthreads implementation
  matrix ptresults[MAX_TEST_CASES];
  printf("Posix-Threads calculation...\n");

  gettimeofday(&start, NULL);
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
    multithreaded(&firsts[i], &seconds[i], &ptresults[i], threads);
  }
  gettimeofday(&end, NULL);

  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
      if(!compareMatrices(&results[i], &ptresults[i]))
      {
        printf("Pthread-Implementation has faults!");
        exit(0);
      }
  }

  fprintf(performance,
    "Pthreads-Implementation took %.3lf seconds for all testcases.\n"
    , getDifference(start, end));

  //cleaning up
  printf("Cleaning up...\n");
  fclose(performance);
  for (int i = 0; i < MAX_TEST_CASES; ++i)
  {
      free(firsts[i].values);
      free(seconds[i].values);
      free(results[i].values);
      free(ompresults[i].values);
      free(ptresults[i].values);
  }

  printf("All done, see %s for a summarization of each implementation's performance.\n"
    , PATH_TO_TIMES);
  exit(0);

}