#include <stdio.h>
#include <stdlib.h>
#include "Convolution.h"
#include <iostream>

int main()
{
    hls::stream<intSdCh> inStream;
    hls::stream<intSdCh> outStream;

    //Array initialisation
    unsigned char picture[HEIGHT][WIDTH];

    //Fill the picture
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            //picture[y][x] = rand() % 255;
            picture[y][x] = (x == y) ? 100 : 0;
        }
    }

    //Send data to the input stream
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            intSdCh aValue;
            aValue.data = picture[y][x];
            aValue.last = 0;
            aValue.strb = -1;
            aValue.keep = 15;
            aValue.user = 0;
            aValue.id = 0;
            aValue.dest = 0;
            inStream.write(aValue);
        }
    }

    //Perform median filter
    convolution(inStream, outStream);

    printf("Result = \n");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            intSdCh valOut;
            outStream.read(valOut);
            printf("%d", (int)valOut.data);
            if (x * y != (WIDTH - 1) * (HEIGHT - 1))
            {
                printf(",");
            }
        }
        printf("\n");
    }

    return 0;
}
