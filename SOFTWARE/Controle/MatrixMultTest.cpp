#include <stdio.h>
#include <stdlib.h>
#include "MatrixMult.h"
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

	//Array initialisations
	float A[DIM][DIM];
	float B[DIM][DIM];
	float C[DIM][DIM];

	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
			A[i][j] = (i==j)? 1:0;
			B[i][j] = i*j;
		}
	}

	//Send A matrix to the input stream
	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
			intSdCh aValue;
			aValue.data = A[i][j];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	//Send B matrix to the input stream
	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
			intSdCh aValue;
			aValue.data = B[i][j];
			aValue.last = (i * j == (DIM - 1) * (DIM - 1)) ? 1 : 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	//Perform multiplication
	matrixMult(inputStream,outputStream);

	//Print results
	printf("Result= \n");
	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
            intSdCh valOut;
            outputStream.read(valOut);
            printf("%d", (int)valOut.data);
			printf(" ");
        }
        printf("\n");
	}

	return 0;
}
