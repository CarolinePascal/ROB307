#include <stdio.h>
#include <stdlib.h>
#include "ICP.h"
#include <iostream>

int main()
{
    hls::stream<intSdCh> inStream;
    hls::stream<intSdCh> outStream;

    //Array initialisations

    float dat[LENGTH_DAT][N_FEATURES];
    float ref[LENGTH_REF][N_FEATURES];

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

    //Send data to the input stream
    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh aValue;
            aValue.data = ref[i][j];
            aValue.last = 0;
            aValue.strb = -1;
            aValue.keep = 15;
            aValue.user = 0;
            aValue.id = 0;
            aValue.dest = 0;
            inStream.write(aValue);
        }
    }

    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh aValue;
            aValue.data = dat[i][j];
            aValue.last = (i + j == LENGTH_REF - 1 + N_FEATURES - 1) ? 1 : 0;
            aValue.strb = -1;
            aValue.keep = 15;
            aValue.user = 0;
            aValue.id = 0;
            aValue.dest = 0;
            inStream.write(aValue);
        }
    }

    //Perform association
    association(inStream, outStream);

    //Display the results
    printf("Result= \n");
    for (int i = 0; i < LENGTH_DAT; i++)
    {
        for (int k = 0; k < N_FEATURES; k++)
        {
            intSdCh valOut;
            outStream.read(valOut);
            printf("%d ", (int)valOut.data);
        }
        printf("\n");
    }

    return 0;
}
