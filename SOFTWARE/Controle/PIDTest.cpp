#include <stdio.h>
#include <stdlib.h>
#include "PID.h"
#include <iostream>
#include <fstream>

int main()
{
    //previous error, cumulated error
    float previous[2];
    //command, error
    float PIDOutput[2];
    //Kp,Ki,Kd,scale,min,max,dt
    static float PIDValues[7] = {1, 1, 1, 1, -2, 10, 0.1};

    volatile float objective = 5;
    volatile float current = 0;

    hls::stream<intSdCh> inputStream;
    hls::stream<intSdCh> outputStream;

    int counter = 0;

    while (counter < 10)
    {
        /************** send data ******************/

        intSdCh aValue;
        aValue.data = current;
        aValue.last = 0;
        aValue.strb = -1;
        aValue.keep = 15;
        aValue.user = 0;
        aValue.id = 0;
        aValue.dest = 0;
        outputStream.write(aValue);

        aValue.data = objective;
        aValue.last = 0;
        aValue.strb = -1;
        aValue.keep = 15;
        aValue.user = 0;
        aValue.id = 0;
        aValue.dest = 0;
        outputStream.write(aValue);

        PID(inputStream, outputStream);

        printf("res={");
        intSdCh valOut;
        outputStream.read(valOut);
        current = valOut.data;
        outputStream.read(valOut);
        printf("%d", (int)valOut.data);
        printf("} \n");
    }
    return 0;
}
