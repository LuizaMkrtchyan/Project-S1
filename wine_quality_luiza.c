#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 1143 // Maximum number of rows in the CSV file
#define MAX_COLS_X 12 // Maximum number of columns in matrix_X (including the column of 1's)
#define MAX_COLS_Y 1  // Maximum number of columns in matrix_Y (last column)
#define MAX_LINE 1000 // Maximum length of a line in the CSV file

// Function to read a CSV file and store data into matrices
void readCSV(const char *filename, double matrix_X[MAX_ROWS][MAX_COLS_X], double matrix_Y[MAX_ROWS], int *rows)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    *rows = 0;

    // Skip the first row (header/labels)
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        fprintf(stderr, "Error reading header\n");
        exit(EXIT_FAILURE);
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *token;
        token = strtok(line, ",");
        int col = 0;

        // Set the first column in matrix_X to 1
        matrix_X[*rows][col++] = 1.0;

        // Parse the comma-separated values in each line
        while (token != NULL && col < MAX_COLS_X)
        {
            matrix_X[*rows][col] = atof(token);
            token = strtok(NULL, ",");
            col++;
        }

        // The last column ->  matrix_Y
        matrix_Y[*rows] = atof(token);

        (*rows)++;
    }

    fclose(file);
}
// Function to transpose a matrix
void transposeMatrix(double inputMatrix[MAX_ROWS][MAX_COLS_X], double outputMatrix[MAX_COLS_X][MAX_ROWS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS_X; j++)
        {
            outputMatrix[j][i] = inputMatrix[i][j];
        }
    }
}
// function to transpose Y vector to Y matrix(column)
void transposeRowToColumn(double vector[MAX_ROWS], double new[MAX_ROWS][1])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        new[i][0] = vector[i];
    }
}
void multiplyMatrices_XT_X(double matrixA[MAX_COLS_X][MAX_ROWS], double matrixB[MAX_ROWS][MAX_COLS_X],
                           double result[MAX_COLS_X][MAX_COLS_X], int rowsA, int colsA, int rowsB, int colsB)
{

    if (colsA != rowsB)
    {
        printf("Error: Matrix multiplication not possible. Number of columns in matrixA must be equal to the number of rows in matrixB.\n");
        return;
    }

    for (int i = 0; i < rowsA; i++)
    {
        for (int j = 0; j < colsB; j++)
        {
            result[i][j] = 0.0;
            for (int k = 0; k < colsA; k++)
            {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}

void multiplyMatrices_XT_Y(double matrixA[MAX_COLS_X][MAX_ROWS], double matrixB[MAX_ROWS][1],
                           double result[MAX_COLS_X][MAX_COLS_Y], int rowsA, int colsA, int rowsB, int colsB)
{

    if (colsA != rowsB)
    {
        printf("Error: Matrix multiplication not possible. Number of columns in matrixA must be equal to the number of rows in matrixB.\n");
        return;
    }

    for (int i = 0; i < rowsA; i++)
    {
        for (int j = 0; j < colsB; j++)
        {
            result[i][j] = 0.0;
            for (int k = 0; k < colsA; k++)
            {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}
void multiplyMatrices(double matrixA[MAX_COLS_X][MAX_COLS_X], double matrixB[MAX_COLS_X][MAX_COLS_Y],
                      double result[MAX_COLS_X][MAX_COLS_Y], int rowsA, int colsA, int rowsB, int colsB)
{

    if (colsA != rowsB)
    {
        printf("Error: Matrix multiplication not possible. Number of columns in matrixA must be equal to the number of rows in matrixB.\n");
        return;
    }

    for (int i = 0; i < rowsA; i++)
    {
        for (int j = 0; j < colsB; j++)
        {
            result[i][j] = 0.0;
            for (int k = 0; k < colsA; k++)
            {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}
int inverseMatrix(double matrix[MAX_COLS_X][MAX_COLS_X], double inverse[MAX_COLS_X][MAX_COLS_X], int size)
{
    // Augment the matrix with an identity matrix
    double augmentedMatrix[MAX_COLS_X][2 * MAX_COLS_X];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            augmentedMatrix[i][j] = matrix[i][j];
            augmentedMatrix[i][j + size] = (i == j) ? 1.0 : 0.0;
        }
    }

    // Perform row operations to get reduced row-echelon form
    for (int i = 0; i < size; i++)
    {
        // Make the diagonal element 1
        double pivot = augmentedMatrix[i][i];
        if (pivot == 0.0)
        {
            printf("Error: Matrix is singular, and its inverse does not exist.\n");
            return 0; // Matrix is singular, and its inverse does not exist
        }

        for (int j = 0; j < 2 * size; j++)
        {
            augmentedMatrix[i][j] /= pivot;
        }

        // Make other elements in the column 0
        for (int k = 0; k < size; k++)
        {
            if (k != i)
            {
                double factor = augmentedMatrix[k][i];
                for (int j = 0; j < 2 * size; j++)
                {
                    augmentedMatrix[k][j] -= factor * augmentedMatrix[i][j];
                }
            }
        }
    }

    // Copy the right half of the augmented matrix to the inverse matrix
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            inverse[i][j] = augmentedMatrix[i][j + size];
        }
    }

    return 1; // Matrix inversion successful
}
int main()
{
    const char *filename = "wine_quality_ds.csv";
    double matrix_X[MAX_ROWS][MAX_COLS_X];
    double matrix_Y[MAX_ROWS];
    int rows;

    // Read the CSV file and store data into matrices
    readCSV(filename, matrix_X, matrix_Y, &rows);

    //Hashvum enq X Transposed-y
    double transposed_matrix_X[MAX_COLS_X][MAX_ROWS];
    transposeMatrix(matrix_X, transposed_matrix_X);

    //BAzmapatkum enq Transposed X-y X-i het
    double XT_X[MAX_COLS_X][MAX_COLS_X];
    multiplyMatrices_XT_X(transposed_matrix_X, matrix_X, XT_X, MAX_COLS_X, MAX_ROWS, MAX_ROWS, MAX_COLS_X);

    //Inverse enq anum et stacac artadryaly(XT*X)
    double inversed_XT_X[MAX_COLS_X][MAX_COLS_X];
    inverseMatrix(XT_X, inversed_XT_X, MAX_COLS_X);

    //dardznum enq Y-y column
    double Y[MAX_ROWS][1];
    transposeRowToColumn(matrix_Y, Y);
    //U bazmapatkum enq XT-i het
    double XT_Y[MAX_COLS_X][MAX_COLS_Y];
    multiplyMatrices_XT_Y(transposed_matrix_X, Y, XT_Y, MAX_COLS_X, MAX_ROWS, MAX_ROWS, MAX_COLS_Y);
    // inversed(XT* X)*(Y*XT)
    double a[MAX_COLS_X][MAX_COLS_Y];
    multiplyMatrices(inversed_XT_X, XT_Y, a, MAX_COLS_X, MAX_COLS_X, MAX_COLS_X, MAX_COLS_Y);

    //Nermucum a user-y chimicat-nery
    double x[11];
    printf("Fixed acidity: ");
    scanf("%lf", &x[0]);
    printf("Volatile acidity: ");
    scanf("%lf", &x[1]);
    printf("Citric acid: ");
    scanf("%lf", &x[2]);
    printf("Residual sugar: ");
    scanf("%lf", &x[3]);
    printf("Chlorides: ");
    scanf("%lf", &x[4]);
    printf("Free sulfur dioxide: ");
    scanf("%lf", &x[5]);
    printf("Total sulfur dioxide: ");
    scanf("%lf", &x[6]);
    printf("Desity: ");
    scanf("%lf", &x[7]);
    printf("pH: ");
    scanf("%lf", &x[8]);
    printf("Sulphates: ");
    scanf("%lf", &x[9]);
    printf("Alcohol: ");
    scanf("%lf", &x[10]);

    // Haytararum enq quality-n
    double quality = a[0][0];

    //quality = a0 + a1*x1 + a2*x2 + a3*x3 +...
    for (int i = 0; i < MAX_COLS_X - 1; i++)
    {
        quality += (a[i + 1][0] * x[i]);
    }

    printf("\nQuality: %lf", quality);

    return 0;
}
