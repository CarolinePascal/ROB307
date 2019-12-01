#include <stdio.h>
#include <stdlib.h>
#include "ICP.h"
#include <iostream>
#include <fstream>

int main()
{
	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

    //Array initialisations

    float dat[LENGTH_DAT][2];
    float ref[LENGTH_REF][2];
    float filteredRef[LENGTH_DAT][2];

    for(int i = 0; i < LENGTH_DAT; i++)
    {
        dat[i][0] = i;
        dat[i][1] = i;
    }

    for(int i = 0; i < LENGTH_REF; i++)
    {
        ref[i][0] = LENGTH_REF - i;
        ref[i][1] = 0;
    }

    //Send data to the input stream
    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            intSdCh aValue;
			aValue.data = ref[i][j];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
        }
    }

    for (int i = 0; i < LENGTH_REF; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            intSdCh aValue;
			aValue.data = dat[i][j];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
        }
    }
	
    //Perform association
	associations(inputStream, outputStream);

    //Display the results
	printf("Result= \n");
	for (int i = 0; i < LENGTH_DAT; i++)
	{
        for(int k = 0; k < N_FEATURES; k++)
        {
            intSdCh valOut;
            outputStream.read(valOut);
            printf("%d", (int)valOut.data);
        }
        print("\n");
	}

	return 0;
}
