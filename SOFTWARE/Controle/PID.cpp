#include "PID.h"

//previous error, cumulated error
float previous[2];

//command, error
float PIDOutput[2];

//Kp,Ki,Kd,scale,min,max,dt
static float PIDValues[7] = {1, 1, 1, 1, -2, 10, 0.1};

volatile float objective;
volatile float current;

void PID(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    intSdCh valRef;

    intSdCh valIn = inStream.read();
    current = valIn.data;
    valRef = valIn;

    valIn = inStream.read();
    objective = valIn.data;

    float max_lim = PIDValues[5];
    float min_lim = PIDValues[4];

    // compute error signal E = W - Y
    float input = objective - previous[0];
    float error = (input > max_lim) ? max_lim : input;
    error = (input < min_lim) ? min_lim : error;

    //Proportionnal
    float proportionnal = PIDValues[0] * error;

    //Derivative
    float derivative = PIDValues[1] * (error - previous[0]) / PIDValues[6];

    //Intration
    float integral = (previous[1] + error * PIDValues[6]);

    float integration = (integral*PIDValues[2] > max_lim) ? max_lim : integral*PIDValues[2];
    integration = (integration < min_lim) ? min_lim : integration;

    //Scale
    float output = PIDValues[3] * (proportionnal + derivative + integration);

    output = (output > max_lim) ? max_lim : output;
    output = (output < min_lim) ? min_lim : output;

    PIDOutput[0] = output; // PID output
    PIDOutput[1] = error;  // error reported as output

    previous[0] = error;
    previous[1] += integral;

    for (int i = 0; i < 2; i++)
    {
        intSdCh valOut;
        valOut.data = PIDOutput[i];
        valOut.keep = valRef.keep;
        valOut.strb = valRef.strb;
        valOut.user = valRef.user;
        valOut.last = (i == 1) ? 1 : 0;
        valOut.id = valRef.id;
        valOut.dest = valRef.dest;
        outStream << valOut;
    }

    return;
}
