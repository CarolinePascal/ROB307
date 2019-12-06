#include <stdio.h>
#include <stdlib.h>
#include "KMeans.h"
#include <iostream>
#include <cstdlib>

int main()
{
	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

	//Input array initialisation
	unsigned char points[N_POINTS][N_FEATURES];

	//For KNN, uncomment here
	//unsigned char clusters[N_POINTS];

	for (int i = 0; i < N_POINTS; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			points[i][j] = rand() % 255;
		}
	}

	//Send data to the input stream
	for (int i = 0; i < N_POINTS; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			intSdCh aValue;
			aValue.data = points[i][j];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	//The first centroids are randomly initialised and sent
	for (int i = 0; i < N_CLUSTER; i++)
	{
		int index = rand() % (N_POINTS - 1);
		for (int j = 0; j < N_FEATURES; j++)
		{
			intSdCh aValue;
			aValue.data = points[index][j];
			aValue.last = (i * j == (N_CLUSTER - 1) * (N_FEATURES - 1)) ? 1 : 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	//Perform clusterisation
	KMeans(inputStream, outputStream);

	printf("Result={");
	for (int i = 0; i < (N_POINTS); i++)
	{
		intSdCh valOut;
		outputStream.read(valOut);
		//For KNN, uncomment here
		//clusters[i] = valOut.data;
		printf("%d", valOut.data);

		if (i != N_POINTS - 1)
		{
			printf(",");
		}
	}
	printf("} \n");

	//For KMeans only, uncomment here
	return (0);
}

/*Code for KNN*/
/*

	//Send data to the input stream
	for (int i = 0; i < N_POINTS; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			intSdCh aValue;
			aValue.data = points[i][j];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	for (int i = 0; i < N_POINTS; i++)
	{
		intSdCh aValue;
		aValue.data = clusters[i];
		aValue.last = 0;
		aValue.strb = -1;
		aValue.keep = 15;
		aValue.user = 0;
		aValue.id = 0;
		aValue.dest = 0;
		inputStream.write(aValue);
	}

	//Classify the first point
	for (int i = 0; i < N_FEATURES; i++)
	{
		intSdCh aValue;
		aValue.data = points[0][i];
		aValue.last = (i == N_FEATURES - 1) ? 1 : 0;
		aValue.strb = -1;
		aValue.keep = 15;
		aValue.user = 0;
		aValue.id = 0;
		aValue.dest = 0;
		inputStream.write(aValue);
	}

	//Perform classification
	KNN(inputStream, outputStream);

	printf("Result={");
	intSdCh valOut;
	outputStream.read(valOut);
	printf("%d", valOut.data);
	printf("}");

	return 0;
}
*/
