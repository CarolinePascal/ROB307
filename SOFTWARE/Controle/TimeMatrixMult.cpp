#include <chrono>
#include <cstdio>
#include <math.h>

#define DIM 32

void matrixMult(float A[DIM][DIM], float B[DIM][DIM], float C[DIM][DIM])
{
//Perform multiplication
LOOP1:
    for (int ia = 0; ia < DIM; ++ia)
    {
    LOOP2:
        for (int ib = 0; ib < DIM; ++ib)
        {
            float sum = 0;

        LOOP3:
            for (int id = 0; id < DIM; ++id)
            {
                sum += A[ia][id] * B[id][ib];
            }

            C[ia][ib] = sum;
        }
    }
}

main()
{
    //Array initialisations
    float A[DIM][DIM];
    float B[DIM][DIM];
    float C[DIM][DIM];

    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            A[i][j] = (i == j) ? 1 : 0;
            B[i][j] = i * j;
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        matrixMult(A, B, C);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = duration / 1000.;

    printf("Code execution time : %f \n", ellapsed);

    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            printf("%f,", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
