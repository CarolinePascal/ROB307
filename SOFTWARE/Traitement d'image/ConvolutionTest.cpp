#include <stdio.h>
#include <stdlib.h>
#include "Convolution.h"
#include <iostream>
#include <fstream>

int picture[WIDTH][HEIGHT];

int main()
{
    hls::stream<intSdCh> inputStream;
    hls::stream<intSdCh> outputStream;

    /*************** load data *****************/
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0, j < HEIGHT, j++)
        {
            picture[i][j] = i + j;
        }
    }

    /************** send data ******************/
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            send_strvalue(inputStream, picture[i][j], 0);
        }
    }

    convolution(outputStream, inputStream);

    printf("res={");
    for (int i = 0; i < (N_POINTS); i++)
    {
        intSdCh valOut;
        outputStream.read(valOut);
        printf("%d", (int)valOut.data);
        if (i != N_POINTS - 1)
        {
            printf(",");
        }
    }
    printf("} \n");
    for (int i = 0; i < 2; i++)
    {
        //		intSdCh valOut;
        //		outputStream.read(valOut);
        //		printf("end : %d ,", (int) valOut.data);
    }
    return 0;
}
