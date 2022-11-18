/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define GRANULARITY 8
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

static void*
init_matrix_row(void* buf)
{
    unsigned long i = (unsigned long) buf;// raden är samma som id
    for(unsigned long i = (unsigned long) buf * (SIZE/GRANULARITY); i < ((unsigned long) buf+1) * (SIZE/GRANULARITY); i++) 
        for (int j = 0; j < SIZE; j++) { // hela columnen ska göras i tråden.
            
            a[i][j] = 1.0; // sätt a till 1
            b[i][j] = 1.0; // sätt b till 1
        }
}
static void
init_matrix_parallel(void)
{
    pthread_t *rows;
    rows = malloc( GRANULARITY * sizeof(pthread_t) );
    for (int i = 0; i < GRANULARITY; i++){
        pthread_create(&(rows[i]), NULL, init_matrix_row, (void*)(__intptr_t)i);
    }
    for (int i = 0; i < GRANULARITY; i++){
        pthread_join(rows[i], NULL);
    }
    free(rows);
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
    unsigned long i = (unsigned long) buf;
    for (i = ((unsigned long) buf*i)*(SIZE/GRANULARITY); i < ((unsigned long) buf+1)*(SIZE/GRANULARITY); i++){
        for (j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
matmul_parallel()
{
    unsigned i;
    pthread_t *rows;
    rows = malloc( GRANULARITY * sizeof(pthread_t) );
    for (i = 0; i < GRANULARITY; i++) {
        pthread_create(&(rows[i]), NULL, matmul_parallel_row, (void*)(__intptr_t)i);
    }
    for (i = 0; i < GRANULARITY; i++) {
        pthread_join(rows[i], NULL);
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
    init_matrix_parallel();
    matmul_parallel();
    //print_matrix();
}
