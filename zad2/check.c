#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>
#define EPSILON 1e-9
#include <immintrin.h> // OPT3
#include <x86intrin.h> // OPT3
#define BLKSIZE 8
#define BLKSIZE2 16

#define IDX(i, j, n) (((j)+ (i)*(n)))

int max(int a, int b){
    if (a > b)
        return (a);
    return (b);
}

int chol1(double *A, unsigned int n){
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

int chol2(double *A, unsigned int n){
    register unsigned int i,j,k; //OPT1
    register double tmp; //OPT1

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)]; // OPT1
            for (k = 0; k < j; k++) {
                tmp -= A[IDX(i, k, n)] * A[IDX(j, k, n)]; //OPT1
            }
            A[IDX(i, j, n)] = tmp; //OPT1
        }

        if (A[IDX(j, j, n)] < 0.0) {
            return (1);
        }

        tmp = sqrt(A[IDX(j, j, n)]); //OPT1
        for (i = j + 1; i < n; i++){
            A[IDX(i, j, n)] /= tmp; //OPT1
        }
        A[IDX(j,j,n)] = tmp; //OPT1
    }

    return (0);
}

int chol3(double *A, unsigned int n){
    register unsigned int i,j,k;
    register double tmp;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE, 0)) { // OPT 2
                    tmp -= A[IDX(i, k, n)] * A[IDX(j, k, n)] +
                           A[IDX(i, k+1, n)] * A[IDX(j, k+1, n)] +
                           A[IDX(i, k+2, n)] * A[IDX(j, k+2, n)] +
                           A[IDX(i, k+3, n)] * A[IDX(j, k+3, n)] +
                           A[IDX(i, k+4, n)] * A[IDX(j, k+4, n)] +
                           A[IDX(i, k+5, n)] * A[IDX(j, k+5, n)] +
                           A[IDX(i, k+6, n)] * A[IDX(j, k+6, n)] +
                           A[IDX(i, k+7, n)] * A[IDX(j, k+7, n)];
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

int chol4(double *A, unsigned int n){
    register unsigned int i,j,k;
    register double tmp;
    register __m128d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // OPT 3

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE, 0)) {
                    tmp0 = _mm_loadu_pd(A+IDX(i, k, n)); // <- OPT 3
                    tmp1 = _mm_loadu_pd(A+IDX(j, k, n));
                    tmp2 = _mm_loadu_pd(A+IDX(i, k+2, n));
                    tmp3 = _mm_loadu_pd(A+IDX(j, k+2, n));
                    tmp4 = _mm_loadu_pd(A+IDX(i, k+4, n));
                    tmp5 = _mm_loadu_pd(A+IDX(j, k+4, n));
                    tmp6 = _mm_loadu_pd(A+IDX(i, k+6, n));
                    tmp7 = _mm_loadu_pd(A+IDX(j, k+6, n));
                    tmp0 = _mm_mul_pd(tmp0, tmp1); // <- OPT 3
                    tmp2 = _mm_mul_pd(tmp2, tmp3);
                    tmp4 = _mm_mul_pd(tmp4, tmp5);
                    tmp6 = _mm_mul_pd(tmp6, tmp7);
                    tmp0 = _mm_add_pd(tmp0, tmp2); // <- OPT 3
                    tmp4 = _mm_add_pd(tmp4, tmp6);
                    tmp0 = _mm_add_pd(tmp0, tmp4);

                    tmp -= tmp0[0] + tmp0[1];
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

int chol5(double *A, unsigned int n){
    register unsigned int i,j,k;
    register double tmp;
    register __m128d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // OPT 3
    register __m128d tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE2, 0)) {
                    tmp0 = _mm_loadu_pd(A+IDX(i, k, n)); // <- OPT 4
                    tmp1 = _mm_loadu_pd(A+IDX(j, k, n));
                    tmp2 = _mm_loadu_pd(A+IDX(i, k+2, n));
                    tmp3 = _mm_loadu_pd(A+IDX(j, k+2, n));
                    tmp4 = _mm_loadu_pd(A+IDX(i, k+4, n));
                    tmp5 = _mm_loadu_pd(A+IDX(j, k+4, n));
                    tmp6 = _mm_loadu_pd(A+IDX(i, k+6, n));
                    tmp7 = _mm_loadu_pd(A+IDX(j, k+6, n));
                    tmp8 = _mm_loadu_pd(A+IDX(i, k+8, n));
                    tmp9 = _mm_loadu_pd(A+IDX(j, k+8, n));
                    tmp10 = _mm_loadu_pd(A+IDX(i, k+10, n));
                    tmp11 = _mm_loadu_pd(A+IDX(j, k+10, n));
                    tmp12 = _mm_loadu_pd(A+IDX(i, k+12, n));
                    tmp13 = _mm_loadu_pd(A+IDX(j, k+12, n));
                    tmp14 = _mm_loadu_pd(A+IDX(i, k+14, n));
                    tmp15 = _mm_loadu_pd(A+IDX(j, k+14, n));

                    tmp0 = _mm_mul_pd(tmp0, tmp1); // <- OPT 5
                    tmp2 = _mm_mul_pd(tmp2, tmp3);
                    tmp4 = _mm_mul_pd(tmp4, tmp5);
                    tmp6 = _mm_mul_pd(tmp6, tmp7);
                    tmp8 = _mm_mul_pd(tmp8, tmp9);
                    tmp10 = _mm_mul_pd(tmp10, tmp11);
                    tmp12 = _mm_mul_pd(tmp12, tmp13);
                    tmp14 = _mm_mul_pd(tmp14, tmp15);

                    tmp0 = _mm_add_pd(tmp0, tmp2); // <- OPT 6
                    tmp4 = _mm_add_pd(tmp4, tmp6);
                    tmp8 = _mm_add_pd(tmp8, tmp10);
                    tmp12 = _mm_add_pd(tmp12, tmp14);

                    tmp0 = _mm_add_pd(tmp0, tmp4);
                    tmp8 = _mm_add_pd(tmp8, tmp12);

                    tmp0 = _mm_add_pd(tmp0, tmp8);

                    tmp -= tmp0[0] + tmp0[1];
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

int chol6(double *A, unsigned int n){
    register unsigned int i,j,k;
    register double tmp;
    register __m256d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // OPT 3
    //register __m128d tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE2, 0)) {
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


int check(double *A, double *B, int size){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++) {
            if (A[IDX(i, j, size)] - B[IDX(i, j, size)] > EPSILON){
                fprintf(stderr, "Error: %f, %f \n", A[IDX(i,j,size)], B[IDX(i,j,size)]);
                return 1;
            }
        }
    }
    return 0;
}


int main(int argc, char ** argv ){

    double* matrix1;
    double* matrix2;
    double* matrix3;
    double* matrix4;
    double* matrix5;
    double* matrix6;

    for (int size = 40; size <= 1000; size += 40) {

        matrix1 = malloc(size * size * sizeof(double));
        matrix2 = malloc(size * size * sizeof(double));
        matrix3 = malloc(size * size * sizeof(double));
        matrix4 = malloc(size * size * sizeof(double));
        matrix5 = malloc(size * size * sizeof(double));
        matrix6 = malloc(size * size * sizeof(double));

        for (int i = 0; i < size; i++) {
            matrix1[IDX(i, i, size)] = 1.0;
            matrix2[IDX(i, i, size)] = 1.0;
            matrix3[IDX(i, i, size)] = 1.0;
            matrix4[IDX(i, i, size)] = 1.0;
            matrix5[IDX(i, i, size)] = 1.0;
            matrix6[IDX(i, i, size)] = 1.0;
        }

        if (chol1(matrix1, size) || chol2(matrix2, size) || chol3(matrix3, size)) {
            fprintf(stderr, "Error: matrix is either not symmetric or not positive definite.\n");
        }
        if (chol4(matrix4, size) || chol5(matrix5, size) || chol6(matrix6, size)) {
            fprintf(stderr, "Error: matrix is either not symmetric or not positive definite.\n");
        }

        if (check(matrix1, matrix2, size) == 1 || check(matrix3, matrix4, size) == 1 || check(matrix5, matrix6, size) == 1 || check(matrix1, matrix3, size) == 1|| check(matrix1, matrix5, size) == 1 ) {
            fprintf(stderr, "Error: Different Output.\n");
            return 1;
        } else {
            fprintf(stderr, "OK.\n");
        }
    }
    return 0;
}