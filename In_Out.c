#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string.h>

#include "In_Out.h"

double* alloc_matrix(int rows, int columns)
{
	return (double*)malloc(sizeof(double)* rows * columns);
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
		while (c!='\n')
		{
			c = fgetc(in);
			if (c == EOF)
			{
				perror("Input file doesnt contain so much lines!");
				return 0;
			}
		}
	}

	//read the first matrix of the line
	if(parseMatricesPart(in, operandA) != 1) return 0;
    
    //return if second matrix shall not be parsed
    if (operandB == NULL) return 1;

	fscanf(in, "%*[ \t]%c", &c);
	if (c == '\n' || c == EOF) //no second matrix in line
	{
		perror("Missing second Matrix!");
        return 0;
	}

	//opening bracket means there is a second matrix in this line
	if (c == '(')
	{
        fseek(in,-1,SEEK_CUR);
		if(parseMatricesPart(in, operandB) != 1) return 0;
		return 1;
	}

	//if c is neither newline or opening bracket, theres a character that doesnt belong there
	perror("Bad Format!");
	return 0;


/*	//here begins the actual parsing
	matrix current;

	//scan dimension
	if(fscanf(in,"(%d x %d)", current.rows, current.columns) != 1)
	{
		perror("Bad Dimension Format!");
		return 0;
	}
	current.values = alloc_matrix(current.rows, current.columns);


	//scan Matrix values
	int c = fgetc(in);

	//todo: ignore every char up to first opening square bracket
	if (c != '[') //matrices are enclosed by square brackets
	{
		perror("Matrices must begin with a '['!.");
		return 0;
	}

	//parsing of matrix values
	double val;
	for (int i = 0; i < (current->rows)*(current->columns); ++i)
	{
		if(fscanf(in,"%lf",&val) != 1)
		{
			perror("Bad Matrix Format!");
			return 0;
		}
		current->values[i] = val;
	}
	if ((c = fgetc(in)) != ']' )
	{
		perror("Matrices must end with a ']'!");
		return 0;
	}

	operandA = *current;


	return 1; */
}

int printMatrix(matrix* M, char* output)
{
    FILE* out;
    int redirect = 0;
    if (strcmp(output,"")) //redirecting standard output to output file
    {
        redirect = 1;
        out = freopen(output, "a", stdout);
        if (out == NULL)
        {
            perror("Output File Error!");
            return 0;
        }
    }
    
    printf("(%d x %d)[", M->columns, M->rows);
   
    for (int i = 0; i<(M->rows * M->columns); ++i)
    {
        printf("%lf ", M->values[i]);
    }
    
    printf("]\n");
    if(redirect) fclose(out);
    return 1;
}
