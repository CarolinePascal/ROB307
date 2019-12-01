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

    //Can be technically modified by another thread => volatile
    volatile float objective = 5;
    volatile float current = 0;
    float command = 0;

    hls::stream<intSdCh> inputStream;
    hls::stream<intSdCh> outputStream;

    int counter = 0;

    //Main loop
    while (counter < 10)
    {
        //Theorically, update is proceeded here
        //current = computeCurrent();
        //objective = computeObjective();
        current += command*0.5; //Arbitrary

        printf("Current = ");
        printf("%f ", current);
        printf("Objective = ");
        printf("%f ", objective);

        counter++;

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

        //Compute new command
        PID(inputStream, outputStream);

        //Display results
        printf("Result={");
        for (int i = 0; i < 2; i++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
            if (i == 0)
            {
                command = (float)valOut.data;
            }
            printf("%f", (float)valOut.data);
            if (i == 0)
            {
                printf(",");
            }    
        }
        printf("} \n");
    }
    return 0;
}