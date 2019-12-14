#include <chrono>
#include <cstdio>

#define N_TESTS 10000

#define DIM 128

void matrixMult(float A[DIM][DIM], float B[DIM][DIM], float C[DIM][DIM])
{
    //Perform multiplication
    for (int ia = 0; ia < DIM; ++ia)
    {
        for (int ib = 0; ib < DIM; ++ib)
        {
            float sum = 0;
            for (int id = 0; id < DIM; ++id)
            {
                sum += A[ia][id] * B[id][ib];
            }

            C[ia][ib] = sum;
        }
    }
}

int main()
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

    for (int i = 0; i < N_TESTS; i++)
    {
        matrixMult(A, B, C);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = (double)duration / (1000. * N_TESTS);

    printf("Code execution time : %f \n", ellapsed);

    /*
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            printf("%f,", C[i][j]);
        }
        printf("\n");
    }
    */
    return 0;
}
