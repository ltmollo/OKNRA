#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>

#define IDX(i, j, n) (((j)+ (i)*(n)))



static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock(){
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    if ( gtod_ref_time_sec == 0.0 )
        gtod_ref_time_sec = ( double ) tv.tv_sec;
    norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}



int chol(double *A, unsigned int n){
    int i, j, k;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            for (k = 0; k < j; k++) {
                A[IDX(i, j, n)] -= A[IDX(i, k, n)] * A[IDX(j, k, n)];
            }
        }

        if (A[IDX(j, j, n)] < 0.0) {
            return (1);
        }

        A[IDX(j, j, n)] = sqrt(A[IDX(j, j, n)]);
        for (i = j + 1; i < n; i++){
            A[IDX(i, j, n)] /= A[IDX(j, j, n)];
        }
    }

    return (0);
}

double calculate_gflops(int size) {
    double flops;
    flops = size * size * size / 3;
    return flops * 1.0e-09;
}


int main() {
    int i, j, reps = 5;
    double dtime, dtime_best = FLT_MAX;
    double *matrix;

    for (int size = 40; size <= 1000; size += 40) {
        matrix = malloc(size * size * sizeof(double));

        for (int rep = 0; rep < reps; rep++) {

            for (i = 0; i < size; i++) {
                matrix[IDX(i, i, size)] = 1.0;
            }

            dtime = dclock();
            if (chol(matrix, size)) {
                fprintf(stderr, "Error: matrix is either not symmetric or not positive definite.\n");
            }
            dtime = dclock() - dtime;

            if (rep == 0) {
                dtime_best = dtime;
            } else {
                dtime_best = (dtime < dtime_best ? dtime : dtime_best);
            }
        }

        double check = 0.0;
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                check = check + matrix[IDX(i, j, size)];
            }
        }
        printf("%d %le %le\n", size, calculate_gflops(size) / dtime_best, check);
        fflush(stdout);
        free(matrix);
    }
}

