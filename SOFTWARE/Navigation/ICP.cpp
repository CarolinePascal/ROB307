#include "ICP.h"

void association(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation
    float ref[LENGTH_REF][N_FEATURES];
    float dat[LENGTH_DAT][N_FEATURES];
    float filteredRef[LENGTH_DAT][N_FEATURES];

    intSdCh valRef;

    READ_REF:
    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            ref[i][j] = valIn.data;
            if (i == 0 && j == 0)
            {
                valRef = valIn;
            }
        }
    }

    READ_DAT:
    for (int i = 0; i < LENGTH_DAT; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            dat[i][j] = valIn.data;
        }
    }

    float distance = 0;
    float distance_min = 99999999999;

    int index = 0;

    DAT:
    for (int i = 0; i < LENGTH_DAT; i++)
    {
        distance_min = 99999999999;
        index = 0;

        REF:
        for (int j = 0; j < LENGTH_REF; j++)
        {   
            distance = 0;
            //Compute distance
            DISTANCE:
            for (int k = 0; k < N_FEATURES; k++)
            {
                distance += (ref[j][k] - dat[i][k])*(ref[j][k] - dat[i][k]);
            }
            //If the distance is smaller, we update it and the corresponding index
            if(distance<distance_min)
            {
                distance_min = distance;
                index = j;
            }
        }

        //We keep only the closest reference points
        CLOSEST:
        for (int k = 0; k < N_FEATURES; k++)
        {
            filteredRef[i][k] = ref[index][k];
        }
    }

    //Send results
    SEND:
    for(int i = 0; i < LENGTH_DAT; i++)
    {
    	for(int j = 0; j < N_FEATURES; j++)
    	{
        	intSdCh valOut;
            valOut.data = filteredRef[i][j];
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (i == LENGTH_DAT-1) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            outStream << valOut;
    	}
    }  
}
