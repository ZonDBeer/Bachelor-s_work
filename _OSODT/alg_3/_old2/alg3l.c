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

int main()
{
         srand(time(NULL));

        double t1 = 0;

        long long test[] = { 600000000, 800000000,
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
                rkey = rand() % 20 +  ( test[j] - 100000000);
                printf("%lli\n", rkey);
                //printf("LUF - %d\n", LUF(rkey, arr, test[j]));
                LUF(rkey, arr, test[j]);
            }
            t1 = wtime() - t1;
            printf("---------------  %.12f\n", t1);
            delete[] arr;
         }

    return 0;
}

