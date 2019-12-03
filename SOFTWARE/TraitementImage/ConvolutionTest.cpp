#include <stdio.h>
#include <stdlib.h>
#include "Convolution.h"
#include <iostream>
#include <fstream>

#include <chrono>

int main()
{
    hls::stream<intSdCh> inputStream;
    hls::stream<intSdCh> outputStream;

    //Array initialisation
    int picture[HEIGHT][WIDTH];

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
            inputStream.write(aValue);
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    //Perform median filter
    convolution(outputStream, inputStream);

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

	printf("Code execution time : %ld \n", duration);


    printf("Result = \n");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
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
