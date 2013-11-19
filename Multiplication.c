//
//  Multiplication.c
//  Test2
//
//  Created by Tom on 19.11.13.
//  Copyright (c) 2013 Tom. All rights reserved.
//

#include <stdio.h>
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


int main(int argc, const char * argv[])
{
    matrix M, N, result;
    parseMatrices("matrices.txt", 0, &M, &N);
    sequential(&M, &N, &result);
    printMatrix(&result, "");
    return 0;
}

