#include <stdio.h>
#include <stdlib.h>
#include "PID.h"
#include <iostream>
#include <fstream>

//PID paramters : Kp,Ki,Kd,scale,min,max,dt
static float PIDValues[7] = {1, 1, 1, 1, -2, 10, 0.1};

int main()
{
    //Initialise arrays

    //Will be updated by the current thread
    //previous error, cumulated error
    float previous[2];
    //command, error
    float PIDOutput[2];

    //Can be technically modified by another thread !
    volatile float objective = 5;
    volatile float current = 0;

    hls::stream<intSdCh> inputStream("inputStream");
    hls::stream<intSdCh> outputStream("outputStream");

    int counter = 0;

    //Main loop
    while (counter < 10)
    {
        printf("Current = ");
        printf("%f ", current);
        printf("Objective = ");
        printf("%f ", objective);

        counter++;
        //current = computeCurrent();
        //objective = computeObjective();

        //Send data to the input stream
        intSdCh aValue;
        aValue.data = current;
        aValue.last = 0;
        aValue.strb = -1;
        aValue.keep = 15;
        aValue.user = 0;
        aValue.id = 0;
        aValue.dest = 0;
        inputStream.write(aValue);

        aValue.data = objective;
        aValue.last = 0;
        aValue.strb = -1;
        aValue.keep = 15;
        aValue.user = 0;
        aValue.id = 0;
        aValue.dest = 0;
        inputStream.write(aValue);

        PID(inputStream, outputStream);

        printf("Result={");
        for (int i = 0; i < 2; i++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
            if (i == 0)
                current = valOut.data;
            printf("%f", (float)valOut.data);
            if (i == 0)
                printf(",");
        }
        printf("} \n");
    }
    return 0;
}