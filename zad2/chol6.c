#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>
#include <immintrin.h> // OPT3

#define IDX(i, j, n) (j+ i*n)
//#define IDX(i,j,n) (i*(i+1)/2+j)
#define BLKSIZE 16


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

int max(int a, int b){
    if (a > b)
        return (a);
    return (b);
}

int chol(double *A, unsigned int n){
    register unsigned int i,j,k;
    register double tmp;
    register __m256d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // OPT 3
    //register __m128d tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE, 0)) {
                    tmp0 = _mm256_loadu_pd(A+IDX(i, k, n)); // <- OPT 4
                    tmp1 = _mm256_loadu_pd(A+IDX(j, k, n));
                    tmp2 = _mm256_loadu_pd(A+IDX(i, k+4, n));
                    tmp3 = _mm256_loadu_pd(A+IDX(j, k+4, n));
                    tmp4 = _mm256_loadu_pd(A+IDX(i, k+8, n));
                    tmp5 = _mm256_loadu_pd(A+IDX(j, k+8, n));
                    tmp6 = _mm256_loadu_pd(A+IDX(i, k+12, n));
                    tmp7 = _mm256_loadu_pd(A+IDX(j, k+12, n));

                    tmp0 = _mm256_mul_pd(tmp0, tmp1); // <- OPT 5
                    tmp2 = _mm256_mul_pd(tmp2, tmp3);
                    tmp4 = _mm256_mul_pd(tmp4, tmp5);
                    tmp6 = _mm256_mul_pd(tmp6, tmp7);

                    tmp0 = _mm256_add_pd(tmp0, tmp2); // <- OPT 6
                    tmp4 = _mm256_add_pd(tmp4, tmp6);

                    tmp0 = _mm256_add_pd(tmp0, tmp4);

                    tmp -= tmp0[0] + tmp0[1] + tmp0[2] + tmp0[3];
                    k += BLKSIZE;
                } else {
                    tmp -= A[IDX(i, k, n)] * A[IDX(j, k, n)];
                    k++;
                }
            }
            A[IDX(i, j, n)] = tmp;
        }

        if (A[IDX(j, j, n)] < 0.0) {
            return (1);
        }

        tmp = sqrt(A[IDX(j, j, n)]);
        for (i = j + 1; i < n; i++){
            A[IDX(i, j, n)] /= tmp;
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
