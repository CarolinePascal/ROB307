#include <stdio.h>
#include <stdlib.h>
#include "Convolution.h"
#include <iostream>

int main()
{
    hls::stream<intSdCh> inputStream;
    hls::stream<intSdCh> outputStream;

    //Array initialisation
    int picture[WIDTH][HEIGHT];

    //Fill the picture
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            //picture[i][j] = rand() % 255;
            picture[i][j] = (i == j) ? 100 : 0;
        }
    }

    //Send data to the input stream
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

    //Perform convolution
    convolutionBuffer(outputStream, inputStream);

    printf("Result = \n");

    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
            printf("%d", (int)valOut.data);
        }
        printf("\n");
    }

    return 0;
}
