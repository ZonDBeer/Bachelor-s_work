#include <cstdio>
#include <cstdlib>
#include <immintrin.h>
#include <sys/time.h>
#include <ctime>

using namespace std;

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

// поиск с использованием интринсиков
int LUF_AVX(double X, double* T, long long NBIG)
{
        long long M = 33;
        long long N = NBIG;
        long long LUF = 0;
        double *storeX = (double*)malloc(4 * sizeof(double));

        if (X > T[NBIG - 1] || N <= 0) {
                free(storeX);
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

                        _mm256_store_pd(storeX+0, cm); // выгрузка в tmpX содержимого вектора cm

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
                        LUF = LUF + J;
                if (K == M)
                        N = N - J;
                else
                        N = L;
        } while (N > 1);
        free(storeX);
        return LUF-1;
}

int main()
{
    srand(time(NULL));

   double t1 = 0;

   long long test[] = { 800000000, 800000000,
                        1000000000, 1200000000,
                        1400000000, 1600000000
                      };

   long long rkey = 0;

   for ( int j = 0; j < 6; j++)
   {
       printf("\n\nTest %lli\n\n", test[j]);
       double *arr = new double[test[j]];
       for (long long i = 0; i < test[j]; i++)
       {
               arr[i] = i;
       }

       t1 = 0;
       t1 = wtime();
       for (int i = 0; i < 10; i++)
       {
           //rkey = rand() % 20 + (test[j] - 100000000);
           //printf("%lli\n", rkey);
           //printf("LUF_AVX - %d\n", LUF_AVX(rkey, arr, test[j]));
           LUF_AVX(test[j] - 2000, arr, test[j]);
       }
       t1 = wtime() - t1;
       printf("---------------  %.12f\n", t1);
       delete[] arr;
    }

    return 0;
}

