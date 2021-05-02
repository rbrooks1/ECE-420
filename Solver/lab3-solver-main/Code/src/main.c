#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "../lib/Lab3IO.h"
#include "../lib/timer.h"

#define EXPECTED_ARGC 2

void print_usage();

void solve(double **A, double *x, int n, long num_threads);

/**
 * Driver code for the solver application. Takes the number of threads as its one command line parameter.
 *
 * Loads an input file containing an augmented matrix A and computes the solution vector x for the equality Ax=b.
 * The value of x and the time required to solve the system of equations are saved to an output file before exiting.
 *
 * @param argc The number of command line arguments given by the user.
 * @param argv An array of strings containing the command line arguments given by the user.
 */
int main(int argc, const char *argv[]) {
    if (argc != EXPECTED_ARGC) {
        print_usage();
        return 1;
    }

    double **A, *x, start, end;
    int size;
    long num_threads = atol(argv[1]);

    // load matrix from file (assume the filename is 'data_input')
    Lab3LoadInput(&A, &size);
    x = malloc(size * sizeof(double));

    // compute x vector by solving the system of equations
    GET_TIME(start)
    solve(A, x, size, num_threads);
    GET_TIME(end)

    // save x to file named 'data_output'
    Lab3SaveOutput(x, size, end - start);
    free(x);

    return 0;
}

/**
 * Prints a message showing how to run the application.
 */
void print_usage() {
    printf("main <number of threads>\n");
}

/**
 * Solves a linear system of equations of the form Ax=b using Gauss-Jordan Elimination.
 *
 * @param A A pointer to an augmented matrix of size n-by-n+1
 * @param x A pointer to the solution vector that is computed in this function.
 * @param n The number of rows in A.
 * @param num_threads The number of threads to use while solving the system.
 */
void solve(double **A, double *x, int n, long num_threads) {
    int i, j, k, l, m, p, max_row;
    double factor;
    // Perform Gaussian elimination to put A into upper triangular form
#pragma omp parallel num_threads(num_threads) default(none) shared(A, x, n, factor) private(i, j, k, l, m, p, max_row)
    {
        for (k = 0; k < n - 1; ++k) {

            // Pivot - swap current row with row containing largest magnitude value
            // element in column k.
#pragma omp single
            {
                max_row = k;

                for (j = k + 1; j < n; ++j) {
                    if (fabs(A[k][k]) < fabs(A[j][k])) { max_row = j; }
                }

                if (k != max_row) {
                    double *tmp = A[k];
                    A[k] = A[max_row];
                    A[max_row] = tmp;
                }
            }

            // Elimination - modify rows below k to "zero" the values in the column k.
#pragma omp for private(i, factor, l) schedule(guided)
            for (i = k + 1; i < n; ++i) {
                factor = A[i][k] / A[k][k];
                for (l = k; l < n + 1; ++l) {
                    A[i][l] -= factor * A[k][l];
                }
            }
        }

        // Perform Jordan elimination to put A into diagonal form
        for (m = n - 1; m > 0; --m) {
#pragma omp for schedule(guided)
            for (i = 0; i < m; ++i) {
                A[i][n] -= A[i][m] / A[m][m] * A[m][n];
                A[i][m] = 0;
            }
        }

// Compute the solution vector x
#pragma omp for schedule(guided)
        for (p = 0; p < n; ++p) {
            x[p] = A[p][n] / A[p][p];
        }
    }
}