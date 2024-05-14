//requires additional changes to the code to make it work
// ge6.c Elsa nie można zmienić
// Nie przyspiesza bo mamy na tyle duży rozmiar macierzy i nie mieści się w cache więc nie jest optymalnie
// przeładowywany

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <x86intrin.h>

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

int ge(double *A, int SIZE)
{
    register int BLKSIZE = 8;
    register int i,j,k;
    register double multiplier;
    register __m128d mm_multiplier;
    register __m128d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;    // To laduje po dwa elementy
    for (k = 0; k < SIZE; k++) {
        for (i = k+1; i < SIZE; i++) {
            multiplier = (A[IDX(i, k, SIZE)]/A[IDX(k,k,SIZE)]);
            mm_multiplier[0] = multiplier;
            mm_multiplier[1] = multiplier;
            for (j = k+1; j < SIZE;) {
                if ( j < (max(SIZE - 8, 0))) {
                    tmp0 = _mm_loadu_pd(A + IDX(i, j, SIZE));
                    tmp2 = _mm_loadu_pd(A + IDX(i, j+2, SIZE));
                    tmp4 = _mm_loadu_pd(A + IDX(i, j+4, SIZE));
                    tmp6 = _mm_loadu_pd(A + IDX(i, j+6, SIZE));

                    tmp1 = _mm_loadu_pd(A + IDX(k, j, SIZE));
                    tmp3 = _mm_loadu_pd(A + IDX(k, j+2, SIZE));
                    tmp5 = _mm_loadu_pd(A + IDX(k, j+4, SIZE));
                    tmp7 = _mm_loadu_pd(A + IDX(k, j+6, SIZE));

                    tmp1 = _mm_mul_pd(tmp1, mm_multiplier);
                    tmp3 = _mm_mul_pd(tmp3, mm_multiplier);
                    tmp5 = _mm_mul_pd(tmp5, mm_multiplier);
                    tmp7 = _mm_mul_pd(tmp7, mm_multiplier);

                    tmp0 = _mm_sub_pd(tmp0, tmp1);
                    tmp2 = _mm_sub_pd(tmp2, tmp3);
                    tmp4 = _mm_sub_pd(tmp4, tmp5);
                    tmp6 = _mm_sub_pd(tmp6, tmp7);

                    _mm_storeu_pd(A + IDX(i,j,SIZE),tmp0);
                    _mm_storeu_pd(A + IDX(i,j+2,SIZE),tmp2);
                    _mm_storeu_pd(A + IDX(i,j+4,SIZE),tmp4);
                    _mm_storeu_pd(A + IDX(i,j+6,SIZE),tmp6);

                    j += BLKSIZE;

                } else {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)]-A[IDX(k, j, SIZE)]*multiplier;  // tutaj nie wiemy co się dzieje
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
