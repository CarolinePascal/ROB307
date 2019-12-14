#include <chrono>
#include <cstdio>
#include <math.h>

#define N_TESTS 10000

#define LENGTH_DAT 16
#define LENGTH_REF 16
#define N_FEATURES 2

void association(float ref[LENGTH_REF][N_FEATURES], float dat[LENGTH_DAT][N_FEATURES], float filteredRef[LENGTH_DAT][N_FEATURES])
{

    float distance = 0;
    float distance_min = 99999999999;

    int index = 0;

DAT:
    for (int i = 0; i < LENGTH_DAT; i++)
    {
        distance_min = 99999999999;
        index = 0;

    REF:
        for (int j = 0; j < LENGTH_REF; j++)
        {
            distance = 0;
        //Compute distance
        DISTANCE:
            for (int k = 0; k < N_FEATURES; k++)
            {
                distance += (ref[j][k] - dat[i][k]) * (ref[j][k] - dat[i][k]);
            }
            //If the distance is smaller, we update it and the corresponding index
            if (distance < distance_min)
            {
                distance_min = distance;
                index = j;
            }
        }

    //We keep only the closest reference points
    CLOSEST:
        for (int k = 0; k < N_FEATURES; k++)
        {
            filteredRef[i][k] = ref[index][k];
        }
    }
}

int main()
{
    float dat[LENGTH_DAT][N_FEATURES];
    float ref[LENGTH_REF][N_FEATURES];
    float filteredRef[LENGTH_DAT][N_FEATURES];

    for (int i = 0; i < LENGTH_DAT; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            dat[i][j] = i;
        }
    }

    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            ref[i][j] = i;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N_TESTS; i++)
    {
        association(ref, dat, filteredRef);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = (double)duration / (1000. * N_TESTS);

    printf("Code execution time : %f \n", ellapsed);

    /*
    for (int i = 0; i < LENGTH_DAT; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            printf("%f,", filteredRef[i][j]);
        }
        printf("\n");
    }
    */

    return 0;
}