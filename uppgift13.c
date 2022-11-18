/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
	        /* Simple initialization, which enables us to easy check
	         * the correct answer. Each element in c will have the same
	         * value as SIZE after the matmul operation.
	         */
	        a[i][j] = 1.0;
	        b[i][j] = 1.0;
        }
}

static void
matmul_seq()
{
    int i, j, k;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void*
matmul_parallel_row(void* buf){
    int j, k;
    unsigned long i = (unsigned long) buf; // i är samma som id
    for (j = 0; j < SIZE; j++) { // alla columner
        c[i][j] = 0.0;
        for (k = 0; k < SIZE; k++)
            c[i][j] = c[i][j] + a[i][k] * b[k][j]; // formel för matrismultiplikation
    }
}

static void
matmul_parallel()
{
    int i;
    pthread_t *rows;
    rows = malloc( SIZE * sizeof(pthread_t) );
    for (i = 0; i < SIZE; i++) {
        pthread_create(&(rows[i]), NULL, matmul_parallel_row, (void*)(__intptr_t)i); // Skapar alla trådar.
    }
    for (i = 0; i < SIZE; i++) {
        pthread_join(rows[i], NULL); // Synkar alla trådar till main processen.
    }
    free(rows);
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	        printf(" %7.2f", c[i][j]);
	    printf("\n");
    }
}

int
main(int argc, char **argv)
{
    init_matrix();
    matmul_parallel();
    //print_matrix();
}
