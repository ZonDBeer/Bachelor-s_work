/*
 *
 * gcc -Wall -O3 -ffast-math -fivopts -march=native -fopt-info-vec -o alg2 alg2.c
 *
 */



///* Compare */
//#define _CMP_EQ_OQ    0x00 /* Equal (ordered, non-signaling)  */
//#define _CMP_LT_OS    0x01 /* Less-than (ordered, signaling)  */
//#define _CMP_LE_OS    0x02 /* Less-than-or-equal (ordered, signaling)  */
//#define _CMP_UNORD_Q  0x03 /* Unordered (non-signaling)  */
//#define _CMP_NEQ_UQ   0x04 /* Not-equal (unordered, non-signaling)  */
//#define _CMP_NLT_US   0x05 /* Not-less-than (unordered, signaling)  */
//#define _CMP_NLE_US   0x06 /* Not-less-than-or-equal (unordered, signaling)  */
//#define _CMP_ORD_Q    0x07 /* Ordered (nonsignaling)   */
//#define _CMP_EQ_UQ    0x08 /* Equal (unordered, non-signaling)  */
//#define _CMP_NGE_US   0x09 /* Not-greater-than-or-equal (unord, signaling)  */
//#define _CMP_NGT_US   0x0a /* Not-greater-than (unordered, signaling)  */
//#define _CMP_FALSE_OQ 0x0b /* False (ordered, non-signaling)  */
//#define _CMP_NEQ_OQ   0x0c /* Not-equal (ordered, non-signaling)  */
//#define _CMP_GE_OS    0x0d /* Greater-than-or-equal (ordered, signaling)  */
//#define _CMP_GT_OS    0x0e /* Greater-than (ordered, signaling)  */
//#define _CMP_TRUE_UQ  0x0f /* True (unordered, non-signaling)  */
//#define _CMP_EQ_OS    0x10 /* Equal (ordered, signaling)  */
//#define _CMP_LT_OQ    0x11 /* Less-than (ordered, non-signaling)  */
//#define _CMP_LE_OQ    0x12 /* Less-than-or-equal (ordered, non-signaling)  */
//#define _CMP_UNORD_S  0x13 /* Unordered (signaling)  */
//#define _CMP_NEQ_US   0x14 /* Not-equal (unordered, signaling)  */
//#define _CMP_NLT_UQ   0x15 /* Not-less-than (unordered, non-signaling)  */
//#define _CMP_NLE_UQ   0x16 /* Not-less-than-or-equal (unord, non-signaling)  */
//#define _CMP_ORD_S    0x17 /* Ordered (signaling)  */
//#define _CMP_EQ_US    0x18 /* Equal (unordered, signaling)  */
//#define _CMP_NGE_UQ   0x19 /* Not-greater-than-or-equal (unord, non-sign)  */
//#define _CMP_NGT_UQ   0x1a /* Not-greater-than (unordered, non-signaling)  */
//#define _CMP_FALSE_OS 0x1b /* False (ordered, signaling)  */
//#define _CMP_NEQ_OS   0x1c /* Not-equal (ordered, signaling)  */
//#define _CMP_GE_OQ    0x1d /* Greater-than-or-equal (ordered, non-signaling)  */
//#define _CMP_GT_OQ    0x1e /* Greater-than (ordered, non-signaling)  */
//#define _CMP_TRUE_US  0x1f /* True (unordered, signaling)

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <malloc.h>
#include <immintrin.h>

#define MAX 50000000//64//900000000           //  MAX t_data size


double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}


int main () {

    __m256 aa, cc;
    float *t_data;

    __m256 bb = _mm256_set_ps ( 450000, 450000, 450000, 450000, 450000, 450000, 450000, 450000 ); // искомое Х
    //__m256 bb = _mm256_load_ps ( xx );

    t_data = (float*) malloc(MAX * sizeof(float));

    if ( t_data == NULL )
    {
        printf("data not allocated");
        exit (1);
     }

    int test[] = {
                   150000000, 300000000, 450000000,
                   600000000, 750000000, 900000000
                  };

    for ( long long int i = 1; i < MAX; i++) {
        t_data[i] = i + 1;
        //printf("%f\n", t_data[i]);
    }

    printf("\n\n");


    double t;

    for ( int k = 0; k < 6; k++ ) {

        t = wtime();

    for ( long long int i = 0; i < MAX; i += 8 ) {

        //aa = _mm256_set_ps (1, 2, 3, 4, 5, 6, 7, 8); // read part of array
        //bb = _mm256_set_ps (2, 3, 4, 4, 6, 7, 8, 9);
        aa = _mm256_load_ps ( &t_data[i] );
        cc = _mm256_sub_ps (aa, bb); // substruction array and X


        // compeare and print answer
        if ( cc[7] < 0 )
        {
            i += 8;
        }
        else
        {

            if ( cc[0] == 0 )
                printf("find 0: number: %f, position : %llu \n", t_data[i], i + 1);
            else
            if ( cc[1] == 0 )
                printf("find 1: number: %f, position : %llu \n", t_data[i + 1], i + 2);
            else
            if ( cc[2] == 0 )
                printf("find 2: number: %f, position : %llu \n", t_data[i + 2], i + 3);
            else
            if ( cc[3] == 0 )
                printf("find 3: number: %f, position : %llu \n", t_data[i + 3], i + 4);
                else
            if ( cc[4] == 0 )
                printf("find 4: number: %f, position : %llu \n", t_data[i + 4], i + 5);
            else
            if ( cc[5] == 0 )
                printf("find 5: number: %f, position : %llu \n", t_data[i + 5], i + 6);
            else
            if ( cc[6] == 0 )
                printf("find 6: number: %f, position : %llu \n", t_data[i + 6], i + 7);
            else
            if ( cc[7] == 0 )
                printf("find 7: number: %f, position : %llu \n", t_data[i + 7], i + 8);
        }


        /*printf("%f\n%f\n%f\n%f\n%f\n%f\n%f\n%f\n\n\n\n",
               cc[0], cc [1], cc[2], cc[3], cc[4], cc[5], cc[6], cc[7]);*/
    }

        t = wtime() - t;

        printf("Elapsed time: %.18f\n\n\n", t);

     }


    free(t_data);
    return 0;
}
