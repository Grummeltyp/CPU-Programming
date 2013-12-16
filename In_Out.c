#include "In_Out.h"

/**
 * Returns the time passed between start and end
 */

double getDifference(struct timeval start, struct timeval end)
{
  return (end.tv_sec - start.tv_sec) + 1E-6 * (end.tv_usec - start.tv_usec);
}

/*
 *compares two matrices A and B. Returns 1 if they are equal, returns 0 otherwise
 */

int compareMatrices(matrix* A, matrix* B)
{
    if (A->rows != B->rows || A->columns != B->columns) return 0;

    for (int i = 0; i<(A->rows)*(A->columns); ++i)
    {
        if (fabs(A->values[i] - B->values[i]) >= EPSILON) return 0;
    }
    return 1;
}

int parseMatricesPart(FILE* input, matrix* M)
{
		//scan dimension
		if(fscanf(input,"(%d x %d)", &(M->columns), &M->rows) != 2)
		{
          //  printf("%d %d", M->rows, M->columns);
            perror("Bad Dimension Format!");
			return 0;
		}
    M->values = alloc_matrix(M->rows, M->columns);

		//scan Matrix values
		int c = fgetc(input);

		//TODO: ignore every char up to first opening square bracket
		if (c != '[') //matrices are enclosed by square brackets
		{
			perror("Matrices must begin with a '['!.");
			return 0;
		}

		//parsing of matrix values
		double val;
		for (int i = 0; i < (M->rows)*(M->columns); ++i)
		{
			if(fscanf(input,"%lf",&val) != 1)
			{
				perror("Bad Matrix Format!");
				return 0;
			}
			M->values[i] = val;
		}

		fscanf(input, "%s", &c);
		if (c != ']' )
		{
			perror("Matrices must end with a ']'!");
			return 0;
		}

		return 1;
}

int parseMatrices(char* input, int line, matrix* operandA, matrix* operandB)
{
	FILE* in = fopen(input,"r");
	if (in == NULL)
	{
		perror("Could not open Input File!");
		return 0;
	}

	//skip lines
	char c = 0;
	for (int i = 0; i < line; ++i)
	{
    c = 0;
		while (c!='\n')
    {
      c = fgetc(in);
    }

	}

	//read the first matrix of the line
	if(parseMatricesPart(in, operandA) != 1)
  {
    fclose(in);
    return 0;
  }
    //return if second matrix shall not be parsed
    if (operandB == NULL)
    {
      fclose(in);
      return 1;
    }
	fscanf(in, "%*[ \t]%c", &c);
	if (c == '\n' || c == EOF) //no second matrix in line
	{
		perror("Missing second Matrix!");
    fclose(in);
    return 0;
	}

	//opening bracket means there is a second matrix in this line
	if (c == '(')
	{
        fseek(in,-1,SEEK_CUR);
		if(parseMatricesPart(in, operandB) != 1)
    {
      fclose(in);
      return 0;
		}
    fclose(in);
    return 1;
	}

	//if c is neither newline or opening bracket, theres a character that doesnt belong there
	perror("Bad Format!");
	return 0;

}

int printMatrix(matrix* M, char* output)
{
    int redirect = 0;
    int newout, stdoutcpy;
    if (strcmp(output,"") != 0) //redirecting standard output to output file
    {
        redirect = 1;
        stdoutcpy = dup(1);
        close(1);
        //because stdout(which is 1) was closed, the new file gets it's descriptor
        if(newout = open(output, O_CREAT|O_APPEND|O_WRONLY, 0777) < 0)
          {
            perror("open: ");
            return 0;
          }
        // if (dup2(1, newout) < 0)
        // {
        //   perror("Redirecting stdout: ");
        //   return 0;
        // }
        // close(newout);
    }

    printf("(%d x %d)[", M->columns, M->rows);

    for (int i = 0; i<(M->rows * M->columns); ++i)
    {
        printf("%lf ", M->values[i]);
    }

    printf("]\n");

    //reopening standard output
    if(redirect)
    {
      if(dup2(stdoutcpy, 1) < 0)
      {
        perror("Restoring stdout: ");
        return 0;
      }
      close(stdoutcpy);
    }
    return 1;
}