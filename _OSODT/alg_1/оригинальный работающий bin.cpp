 
// Search.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <immintrin.h>

using namespace std;

// BINARY SEARCH - SIMPLE
int BSearch(int *data, int N, int key) {
        int l = -1;
        int u = N;
        int p;
        while (l + 1 != u) {
                int m = (l + u) / 2;
                if (data[m] < key)
                        l = m;
                else
                        u = m;
                p = u;
        }
        return p;
}

// BINARY SEARCH - VECTORIZED
int BSearch_AVX(double *data, long long N, double key)
{
        double powerOfTwo = pow(2, floor(log2(N - 1))); // biggest power of 2 smaller than N
        long long splitIndex = N - powerOfTwo; // used to divide a problem into two problems
        double splitValue = data[splitIndex]; // with sizes guaranteed to be powers of 2
        long long p = splitIndex * (key >= splitValue);
        double *store_p = new double;
        *store_p = p;
        if (data[N - 1] < key)
                return -1;
        for (long long j = powerOfTwo / 2; j >= 1; j /= 2) {
                __m256d xm_jvec = _mm256_set_pd(j + j / 2 + j / 4 + j / 8, j + j / 2 + j / 4, j + j / 2, j);
                __m256d xm_idxvec = _mm256_set1_pd(p);
                double cmpval0 = data[p + j];
                double cmpval1 = data[p + j + j / 2];
                double cmpval2 = data[p + j + j / 2 + j / 4];
                double cmpval3 = data[p + j + j / 2 + j / 4 + j / 8];
                __m256d xm_cmpvalvec = _mm256_set_pd(cmpval3, cmpval2, cmpval1, cmpval0);
                __m256d xm_valvec = _mm256_set1_pd(key);
                xm_idxvec = _mm256_add_pd(
                                xm_idxvec, _mm256_andnot_pd(
                                        _mm256_cmp_pd(xm_valvec, xm_cmpvalvec, _CMP_LT_OQ),
                                        xm_jvec));
                _mm256_store_pd((store_p+0), xm_idxvec);
                for (long long i = 0; i < 4; i++) {
                        if (key >= store_p[i])
                        {
                                p = store_p[i];
                                break;
                        }
                }
        }
        if (key != data[p])
                return -1;
        return p;
}


int main()
{
        double key = 0;
        long long N = 0;
        printf("X: ");
        scanf("%lf", &key);
        printf("N: ");
        scanf("%lli", &N);
        double *data = new double[N];
        for (long long i = 0; i < N; i++) {
                data[i] = i;
        }

        //printf("%lf\n", BSearch(data, N, key));
        printf("%lf\n", BSearch_AVX(data, N, key));

        delete [] data;
    return 0;
}

