//#include "stdafx.h"
//#include <intrin.h>
//#include <conio.h>

#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include <sys/time.h>
#include <immintrin.h>

#define MAX 10000000                                       //  MAX t_data size

using namespace std;

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

void mask_avr(double X, double* T, int NBIG)
{

        double *xx = new double[2];
        double *Tx = new double[2];
        for (int i = 0; i < NBIG-1; i+=2) {
                __m128d a;
                __m128d x;
                Tx[0] = T[i];
                Tx[1] = T[i + 1];
                a = _mm_load_pd(Tx);
                xx[0] = X;
                xx[1] = X;
                x = _mm_load_pd(xx);
                __m128d cm = _mm_max_pd(a, x);
                _mm_store_pd(xx, cm);
                T[i] = xx[0];
                T[i+1] = xx[1];
        }
        delete[] xx;
        delete[] Tx;
        for (int i = 0; i < NBIG; i++)
            printf("%i - %f\n", i, T[i]);
}

// поиск с использованием интринсиков
int LUF_AVX(double X, double* T, int NBIG)
{
        int M = 33;
        int N = NBIG;
        int LUF = 0;
        if (N <= 0) return -1;
        do {
                int L = (N - 1) / (M - 1);
                if (L == 0)
                {
                        int K = 0;
                        // _mm_max_pd сравнивает только два элемента вектора
                        double *tmpX = new double[2]; // два элемента равных Х
                        double *tmpT = new double[2]; // два элемента из массива Т
                        for (int i = LUF; i < NBIG-1; i+=2)
                        {
                                __m128d mT; // вектор элементов равных Х
                                __m128d mX; // вектор элементов массива Т

                                tmpT[0] = T[i];
                                tmpT[1] = T[i + 1];
                                mT = _mm_load_pd(tmpT); // загрузка выбранных элементов в вектор mT

                                tmpX[0] = X;
                                tmpX[1] = X;
                                mX = _mm_load_pd(tmpX); // загрузка элементов равных Х в вектор mX

                                // если Х больше элемента вектора mT, то под соответствующим индексом в весторе cm
                                __m128d cm = _mm_max_pd(mT, mX); // будет записан Х, иначе будет записан соответствующий элемент вектора mT

                                _mm_store_pd(tmpX, cm); // выгрузка в tmpX содержимого вектора cm

                                // т.е. если Х остается максимальным значением на данном промежутке
                                // инкрементируем К
                                if (tmpX[0] == X) {
                                        K = K + 1;
                                }
                                if (tmpX[1] == X) {
                                        K = K + 1;
                                }
                        }
                        delete[] tmpX;
                        delete[] tmpT;
                        LUF = LUF + K;
                        return LUF;
                }
                int K = 0;
                // _mm_max_pd сравнивает только два элемента вектора
                double *tmpX = new double[2]; // два элемента равных Х
                double *tmpT = new double[2]; // два элемента из массива Т
                for (int i = LUF; i < N-1; i += L*2)
                {
                        __m128d mT; // вектор элементов равных Х
                        __m128d mX; // вектор элементов массива Т

                        tmpT[0] = T[i];
                        tmpT[1] = T[i + L];
                        mT = _mm_load_pd(tmpT); // загрузка выбранных элементов в вектор mT

                        tmpX[0] = X;
                        tmpX[1] = X;
                        mX = _mm_load_pd(tmpX); // загрузка элементов равных Х в вектор mX

                                                                        // если Х больше элемента вектора mT, то под соответствующим индексом в весторе cm
                        __m128d cm = _mm_max_pd(mT, mX); // будет записан Х, иначе будет записан соответствующий элемент вектора mT

                        _mm_store_pd(tmpX, cm); // выгрузка в tmpX содержимого вектора cm

                        // т.е. если Х остается максимальным значением на данном промежутке
                        // инкрементируем К
                        if (tmpX[0] == X) {
                                K = K + 1;
                        }
                        if (tmpX[1] == X) {
                                K = K + 1;
                        }
                }
                delete[] tmpX;
                delete[] tmpT;

                int J = 1 + (K - 1)*L;
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
int LUF(double X, double T[], int NBIG)
{
        int M = 33;
        int N = NBIG;
        int LUF = 0;
        if (N <= 0) return -1;
        do {
                int L = (N - 1)/(M - 1);
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
                        return LUF;
                }
                int K = 0;
                for (int i = LUF; i < N; i += L)
                {
                        if (T[i] <= X) {
                                K = K + 1;
                        }
                        else break;
                }
                int J = 1 + (K - 1)*L;
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
        int x = 9999990;

        double *t_data;
        double t;

        t_data = (double*)malloc(MAX * sizeof(double));

        //double *arr = new double[MAX];  // 600

        for (int i = 0; i < MAX; i++)   // 600
        {
                t_data[i] = i;
        }

        t = wtime();
        int z = LUF_AVX(x, t_data/*arr*/, MAX);
        printf("LUF_AVX - %d\n", z);
        printf("Data[x] = %f\n", t_data[z]);
        t = wtime() - t;

        printf("%.12f\n\n", t);

        t = wtime();
        int z1 = LUF(x, t_data/*arr*/, MAX);
        printf("LUF - %d\n", z1);
        printf("Data[x] = %f\n", t_data[z1]);
        t = wtime() - t;

        printf("%.12f\n\n", t);

        //system("pause");

    return 0;
}
