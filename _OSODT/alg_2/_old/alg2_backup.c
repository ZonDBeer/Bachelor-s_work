#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/time.h>
#include <immintrin.h>

//#define MAX 150000000//0                                       //  MAX t_data size

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

int linear_search_ref(const double *A, double key, int n)
{
        int result = -1;
        int i;

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

int linear_search(const double *A, double key, int n)
{
#define VEC_INT_ELEMS 4
#define BLOCK_SIZE (VEC_INT_ELEMS * 32)
#define CODE_OF_NOT_EQ 15
        const __m256d vkey = _mm256_set1_pd(key);
        int vresult = -1;
        int result = -1;
        int i, j;
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
        /*double x = 37485;
        double arr[500000];
        for (int i = 0; i < 50000; i++)
                arr[i] = i;
        int n = 500000;*/

        double x = 15000;//99;//3748545;

        double *t_data;

        long long MAX = 150000000;

        t_data = (double*)malloc(MAX * sizeof(double));

        long long i = 0;
        for ( i = 0; i < MAX; i++)
            t_data[i] = i;
        printf("%lli\n", i);
        printf("%lli\n%lf\n\n", MAX, t_data[MAX - 1]);

        double t = 0;

        t = wtime();
        /*printf("%d\n", linear_search(t_data, x, MAX));
        printf("%d\n", linear_search_ref(t_data, x, MAX));*/
        printf("%lf\n", linear_search(t_data, x, MAX));
        printf("%lf\n", linear_search_ref(t_data, x, MAX));
        t = wtime() - t;

        printf("%.12f\n", t);

        free(t_data);

    return 0;
}

