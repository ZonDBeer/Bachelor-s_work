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

// для проверки работы функции поиска максимума
/*void mask_avr(double X, double* T, int NBIG)
{

        double *xx = new double[2];
        double *Tx = new double[2];
        for (int i = 0; i < NBIG-1; i+=2) {
                __m256d a;
                __m256d x;
                Tx[0] = T[i];
                Tx[1] = T[i + 1];
                a = _mm256_load_pd(Tx);
                xx[0] = X;
                xx[1] = X;
                x = _mm256_load_pd(xx);
                __m256d cm = _mm256_max_pd(a, x);
                _mm256_store_pd(xx, cm);
                T[i] = xx[0];
                T[i + 1] = xx[1];
        }
        delete[] xx;
        delete[] Tx;
        for (int i = 0; i < NBIG; i++) printf("%i - %f\n", i, T[i]);
}*/

// поиск с использованием интринсиков
int LUF_AVX(double X, double* T, long long NBIG)
{
        long long M = 33;
        long long N = NBIG;
        long long LUF = 0;
        if (N <= 0) return -1;
        do {
                long long L = (N - 1) / (M - 1);

                if (L == 0)
                {
                        long long K = 0;
                        // _mm_max_pd сравнивает только два элемента вектора
                        //double *tmpX = new double[2]; // два элемента равных Х
                        //double *tmpT = new double[2]; // два элемента из массива Т
                        double *tmpX = (double*)malloc(2 * sizeof(double)); // два элемента равных Х
                        double *tmpT = (double*)malloc(2 * sizeof(double)); // два элемента из массива Т
                        for (long long i = LUF; i < NBIG-1; i+=2)
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

                                _mm256_store_pd(tmpX, cm); // выгрузка в tmpX содержимого вектора cm

                                // т.е. если Х остается максимальным значением на данном промежутке
                                // инкрементируем К
                                if (tmpX[0] == X) {
                                        K = K + 1;
                                }
                                if (tmpX[1] == X) {
                                        K = K + 1;
                                }

                        }
                        free(tmpX);
                        free(tmpT);

                        LUF = LUF + K;
                        return LUF;
                }
                long long K = 0;
                // _mm_max_pd сравнивает только два элемента вектора
                double *tmpX = (double*)malloc(2 * sizeof(double)); // два элемента равных Х
                double *tmpT = (double*)malloc(2 * sizeof(double)); // два элемента из массива Т

                for (long long i = LUF; i < LUF+N-1; i += L*2)
                {
                        __m256d mT; // вектор элементов равных Х
                        __m256d mX; // вектор элементов массива Т

                        tmpT[0] = T[i];
                        if(i + L < NBIG)
                                tmpT[1] = T[i + L];
                        else tmpT[1] = T[i];

                        mT = _mm256_load_pd(tmpT); // загрузка выбранных элементов в вектор mT

                        tmpX[0] = X;
                        tmpX[1] = X;
                        mX = _mm256_load_pd(tmpX); // загрузка элементов равных Х в вектор mX

                                                                        // если Х больше элемента вектора mT, то под соответствующим индексом в весторе cm
                        __m256d cm = _mm256_max_pd(mT, mX); // будет записан Х, иначе будет записан соответствующий элемент вектора mT

                        _mm256_store_pd(tmpX, cm); // выгрузка в tmpX содержимого вектора cm

                        // т.е. если Х остается максимальным значением на данном промежутке
                        // инкрементируем К
                        if (tmpX[0] == X) {
                                K = K + 1;
                        }
                        if (tmpX[1] == X && i+L < NBIG) {
                                K = K + 1;
                        }
                }
                free(tmpX);
                free(tmpT);

                long long J = 1 + (K - 1)*L;
                if (K == 0)
                        return LUF;
                else
                        LUF = LUF + J;
                if (K == M)
                        N = N - J;
                else
                        N = L - 1;
        } while (N > 1);
        return LUF;
}

// поиск элемента с делением интервала на М частей
int LUF(double X, double T[], long long NBIG)
{
        long long M = 33;
        long long N = NBIG;
        long long LUF = 0;
        if (N <= 0) return -1;
        do {
                long long L = (N - 1)/(M - 1);
                if (L == 0)
                {
                        long long K = 0;
                        for (long long i = LUF; i < NBIG; i++)
                        {
                                if (T[i] <= X) {
                                        K = K + 1;
                                }
                                else break;
                        }
                        LUF = LUF + K;
                        return LUF;
                }
                long long K = 0;
                for (long long i = LUF; i < LUF+N; i += L)
                {
                        if (T[i] <= X) {
                                K = K + 1;
                        }
                        else break;
                }
                long long J = 1 + (K - 1)*L;
                if (K == 0)
                        return LUF;
                else
                        LUF = LUF + J;
                if (K == M)
                        N = N - J;
                else
                        N = L - 1;
        } while (N > 1);
        return LUF;
}


int main()
{
        double x = 7500;
        long long i = 0;
        long long MAX = 8000;
        double *arr = (double*)malloc(MAX * sizeof(double));

        for ( i = 0; i < MAX; i++ )
        {
                arr[i] = i;
        }

        double t = 0;

        t = wtime();
        printf("LUF_AVX - %d\n", LUF_AVX(x, arr, MAX));
        t = wtime() - t;

        printf("%.12f\n", t);

        /*t = 0;
        t = wtime();
        printf("LUF - %d\n", LUF(x, arr, MAX));
        t = wtime() - t;

        printf("%.12f\n", t);*/

        free(arr);

    return 0;
}

