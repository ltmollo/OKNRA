//requires additional changes to the code to make it work
// ge4.c wolniej działa :(
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define IDX(i, j, n) (((j)+ (i)*(n)))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double max(double a, double b) {
    return a > b ? a : b;
}

double dclock()
{
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    if ( gtod_ref_time_sec == 0.0 )
        gtod_ref_time_sec = ( double ) tv.tv_sec;
    norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}

int ge(double * A, int SIZE)
{
    register int BLKSIZE = 8;
    register int i,j,k;
    for (k = 0; k < SIZE; k++) {
        register double temp1 = A[IDX(k, k, SIZE)];
        for (i = k+1; i < SIZE; i++) {
            register double multiplier = A[IDX(i, k, SIZE)] / temp1;
            for (j = k+1; j < SIZE;) {
                if ( j < (max(SIZE - 8, 0))) {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)]-A[IDX(k, j, SIZE)]*multiplier;
                    A[IDX(i, j+1, SIZE)] = A[IDX(i, j+1, SIZE)]-A[IDX(k, j+1, SIZE)]*multiplier;
                    A[IDX(i, j+2, SIZE)] = A[IDX(i, j+2, SIZE)]-A[IDX(k, j+2, SIZE)]*multiplier;
                    A[IDX(i, j+3, SIZE)] = A[IDX(i, j+3, SIZE)]-A[IDX(k, j+3, SIZE)]*multiplier;
                    A[IDX(i, j+4, SIZE)] = A[IDX(i, j+4, SIZE)]-A[IDX(k, j+4, SIZE)]*multiplier;
                    A[IDX(i, j+5, SIZE)] = A[IDX(i, j+5, SIZE)]-A[IDX(k, j+5, SIZE)]*multiplier;
                    A[IDX(i, j+6, SIZE)] = A[IDX(i, j+6, SIZE)]-A[IDX(k, j+6, SIZE)]*multiplier;
                    A[IDX(i, j+7, SIZE)] = A[IDX(i, j+7, SIZE)]-A[IDX(k, j+7, SIZE)]*multiplier;
                    j += BLKSIZE;
                } else {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)]-A[IDX(k, j, SIZE)]*multiplier;
                    j++;
                }
            }
        }
    }
    return 0;
}

int main( int argc, const char* argv[] )
{
    register int i,j,k,iret;
    double dtime;
    int SIZE = 1500;
    double* matrix;
    matrix = malloc(SIZE * SIZE * sizeof(double));
    srand(1);
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            matrix[IDX(i, j, SIZE)] = rand();
        }
    }
    printf("call GE");
    dtime = dclock();
    iret = ge(matrix, SIZE);
    dtime = dclock()-dtime;
    printf("Time: %le \n", dtime);

    double check=0.0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            check = check + matrix[IDX(i, j, SIZE)];
        }
    }
    printf( "Check: %le \n", check);
    fflush( stdout );


    return iret;
}
