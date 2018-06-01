/*
 *
 *
 * gcc -Wall -mavx -O3 -ffast-math -fivopts -march=native -fopt-info-vec -fopt-info-vec-missed -o alg3c alg3.c
 *
 * icc -Wall -O3 -xHost -qopt-report3 -qopt-report-phase=vec -o alg3icc alg3.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <sys/time.h>
#include <time.h>


double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

// поиск элемента с делением интервала на М частей
int LUF(double X, double T[], int NBIG)
{
        int M = 33;
        int N = NBIG;
        int LUF = 0;
        if (X > T[NBIG - 1] || N <= 0) return -1;
        do {
                int L = (N - 1) / M;
                if (L == 0)
                {
                        int K = 0;
                        for (int i = LUF; i < NBIG; i++)
                        {
                                if (T[i] <= X) {
                                        K = K + 1;
                                }
                                else break;
                        }
                        LUF = LUF + K;
                        return LUF-1;
                }
                int K = 0;
                for (int i = LUF; i < NBIG; i += L)
                {
                        if (T[i] <= X) {
                                K = K + 1;
                        }
                        else break;
                }
                int J = 1 + (K - 1)*L;
                if (K == 0)
                        return LUF-1;
                else
                        LUF = LUF + J;
                if (K == M)
                        N = N - J;
                else
                        N = L;
        } while (N >= 1);
        return LUF-1;
}

int LUF_AVX(double X, double* T, long long NBIG)
{
        long long M = 33;
        long long N = NBIG;
        long long LUF = 0;
        double storeX[4];

        if (X > T[NBIG - 1] || N <= 0) {
                return -1;
        }
        do {
                long long L = (N - 1) / M;

                if (L == 0)
                {
                        long long K = 0;
                        // _mm_max_pd сравнивает только два элемента вектора
                        //double *tmpX = new double[2]; // два элемента равных Х
                        //double *tmpT = new double[2]; // два элемента из массива Т
                        double tmpX[2]; // два элемента равных Х
                        double tmpT[2]; // два элемента из массива Т
                        for (long long i = LUF; i < NBIG - 1; i += 2)
                        {
                                __m256d mT; // вектор элементов равных Х
                                __m256d mX; // вектор элементов массива Т

                                tmpT[0] = T[i];
                                tmpT[1] = T[i + 1];
                                mT = _mm256_load_pd(tmpT); // загрузка выбранных элементов в вектор mT

                                tmpX[0] = X;
                                tmpX[1] = X;
                                mX = _mm256_load_pd(tmpX); // загрузка элементов равных Х в вектор mX

                                                                                   // если Х больше элемента вектора mT, то под соответствующим индексом в весторе cm
                                __m256d cm = _mm256_max_pd(mT, mX); // будет записан Х, иначе будет записан соответствующий элемент вектора mT

                                _mm256_store_pd(storeX, cm); // выгрузка в tmpX содержимого вектора cm

                                                                                   // т.е. если Х остается максимальным значением на данном промежутке
                                                                                   // инкрементируем К
                                if (storeX[0] == X) {
                                        K = K + 1;
                                }
                                if (storeX[1] == X) {
                                        K = K + 1;
                                }

                        }

                        LUF = LUF + K;
                        return LUF-1;
                }
                long long K = 0;
                // _mm_max_pd сравнивает только два элемента вектора
                double tmpX[2]; // два элемента равных Х
                double tmpT[2]; // два элемента из массива Т


                for (long long i = LUF; i < LUF + N - 1; i += L * 2)
                {
                        __m256d mT; // вектор элементов равных Х
                        __m256d mX; // вектор элементов массива Т

                        tmpT[0] = T[i];
                        if (i + L < NBIG)
                                tmpT[1] = T[i + L];
                        else tmpT[1] = T[i];

                        mT = _mm256_load_pd(tmpT); // загрузка выбранных элементов в вектор mT

                        tmpX[0] = X;
                        tmpX[1] = X;
                        mX = _mm256_load_pd(tmpX); // загрузка элементов равных Х в вектор mX

                                                                           // если Х больше элемента вектора mT, то под соответствующим индексом в весторе cm
                        __m256d cm = _mm256_max_pd(mT, mX); // будет записан Х, иначе будет записан соответствующий элемент вектора mT

                        _mm256_store_pd(storeX, cm); // выгрузка в tmpX содержимого вектора cm

                                                                           // т.е. если Х остается максимальным значением на данном промежутке
                                                                           // инкрементируем К
                        if (storeX[0] == X) {
                                K = K + 1;
                        }
                        if (storeX[1] == X && i + L < NBIG) {
                                K = K + 1;
                        }
                }

                long long J = 1 + (K - 1)*L;
                if (K == 0)
                        return LUF-1;
                else
                        LUF = LUF;
                if (K == M)
                        N = N - J;
                else
                        N = L + 1;
        } while (N > 1);
        return LUF-1;
}

int main()
{
        double key = 0;
        long long N = 0;
        double t = 0, t1 = 0;

        printf("X: ");
        scanf("%lf", &key);
        printf("N: ");
        scanf("%lld", &N);

        double *arr = (double*)malloc(N * sizeof(double));

        for(long long i = 0; i < N; i++)
            arr[i] = i;

        t = wtime();
        for ( int i = 0; i < 50; i++ )
        {
            printf("LUF - %lld\n", LUF(key, arr, N));
        }
        t = wtime() - t;
        printf("LUF = %.12f\n\n", t);

        t1 = 0;
        t1 = wtime();
        for ( int i = 0; i < 50; i++ )
        {
            printf("LUF_AVX - %lld\n", LUF_AVX(key, arr, N));
        }
        t1 = wtime() - t1;
        printf("AVX = %.12f\n", t1);
        printf("LUF = %.12f\n\n", t);

        free(arr);

    return 0;
}
