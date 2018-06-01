/*
 *
 * c++ -Wall -mavx -O3 -ffast-math -fivopts -march=native -fopt-info-vec -fopt-info-vec-missed -c alg2.c -o alg2.o
 * c++ -O3 -mavx -o alg2 alg2.o
 *
 *
 * icc -O3 -xHost -qopt-report3 -qopt-report-phase=vec -o alg2icc alg2.c // vectorize
 * icc -O3 -o alg2icc alg2icc.o
 *
 * clang -O3 -mavx -ffast-math -fvectorize -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -o alg2clang alg2.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/time.h>
#include <immintrin.h>

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

int linear_search_ref(const double *A, double key, long long n)
{
        long long result = -1;
        long long i;

        for (i = 0; i < n; ++i)
        {
                if (A[i] >= key)
                {
                        result = i;
                        break;
                }
        }
        return result;
}

int linear_search(const double *A, double key, long long n)
{
#define VEC_INT_ELEMS 4
#define BLOCK_SIZE (VEC_INT_ELEMS * 32)
#define CODE_OF_NOT_EQ 15
        const __m256d vkey = _mm256_set1_pd(key);
        long long vresult = -1;
        long long result = -1;
        long long i, j;
        bool breakf = false;

        for (i = 0; i <= n - BLOCK_SIZE; i += BLOCK_SIZE)
        {
                __m256d vmask0 = _mm256_set1_pd(-1);
                __m256d vmask1 = _mm256_set1_pd(-1);
                int mask0, mask1;

                for (j = 0; j < BLOCK_SIZE; j += VEC_INT_ELEMS * 2)
                {
                        __m256d vA0 = _mm256_set1_pd(A[i + j]);
                        __m256d vA1 = _mm256_set1_pd(A[i + j + VEC_INT_ELEMS]);
                        __m256d vcmp0 = _mm256_cmp_pd(vkey, vA0, _CMP_GE_OS);
                        __m256d vcmp1 = _mm256_cmp_pd(vkey, vA1, _CMP_GE_OS);
                        vmask0 = _mm256_and_pd(vmask0, vcmp0);
                        vmask1 = _mm256_and_pd(vmask1, vcmp1);

                }
                mask0 = _mm256_movemask_pd(vmask0);
                mask1 = _mm256_movemask_pd(vmask1);
                if ((mask0 & mask1) != CODE_OF_NOT_EQ)
                {
                        vresult = i;
                        break;
                }
        }
        if (vresult > -1)
        {
                result = vresult + linear_search_ref(&A[vresult], key, BLOCK_SIZE);
        }
        else if (i < n)
        {
                result = i + linear_search_ref(&A[i], key, n - i);
        }
        return result;
#undef BLOCK_SIZE
#undef VEC_INT_ELEMS
}

int main()
{
        double x = 0;
        long long MAX = 1000000000;
        long long i = 0;

        double t = 0, t1 = 0;
        double *t_data;



        printf("Enter X: ");
        scanf("%lf", &x);
        printf("X = %lf\n\n", x);

        printf("Enter MAX table size: ");
        scanf("%lli", &MAX);
        printf("MAX = %lli\n\n", MAX);

        t_data = (double*)malloc(MAX * sizeof(double));

        for ( i = 0; i < MAX; i++)
            t_data[i] = i;

        t = wtime();
        for ( int j = 0; j < 500; j++ )
        {

            printf("X = %d\n", linear_search(t_data, x, MAX));

         }
        t = wtime() - t;
        printf("Vec = %.12f\n\n", t);

        t1 = 0;
        t1 = wtime();
        for ( int j = 0; j < 500; j++ )
        {
            printf("Lin = %d\n", linear_search_ref(t_data, x, MAX));
         }
        t1 = wtime() - t1;
        printf("Lin = %.12f\n\n", t1);

        free(t_data);

    return 0;
}
