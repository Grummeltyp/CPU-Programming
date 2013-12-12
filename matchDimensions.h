#indef MATCHDIMENSIONS_H_
#define MATCHDIMENSIONS_H_

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

#endif