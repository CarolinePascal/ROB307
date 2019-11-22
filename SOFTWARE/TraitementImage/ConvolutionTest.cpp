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
        for (int j = 0; j < HEIGHT; j++)
        {
            picture[i][j] = i + j;
        }
    }

    /************** send data ******************/
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            intSdCh aValue;
            aValue.data = picture[i][j];
            aValue.last = 0;
            aValue.strb = -1;
            aValue.keep = 15;
            aValue.user = 0;
            aValue.id = 0;
            aValue.dest = 0;
            inputStream.write(aValue);
        }
    }

    convolution(outputStream, inputStream);

    printf("res={");
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
            printf("%d", (int)valOut.data);
            if (i != (WIDTH - 1) * (HEIGHT - 1))
            {
                printf(",");
            }
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
