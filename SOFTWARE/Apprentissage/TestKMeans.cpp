#include <stdio.h>
#include <stdlib.h>
#include "KMeans.h"
#include <iostream>
#include <fstream>

#include <vector>
#include <string>

using namespace std;

static char FILE_NAME[32] = "TEST.csv";

//For memomry reasons the points array is defined here
static int points[N_POINTS][N_FEATURES];

int main()
{
	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

	//File pointer
	fstream fin;

	//Open an existing file
	fin.open("temp.csv", ios::in);

	//Read the Data from the file as String Vector
	string line;
	int value;
	int counter = 0;

	while (getline(fin, line, '\n'))
	{
		std::stringstream ss(line);

		while (getline(ss, line, ';'))
		{
			if (sscanf(line.c_str(), "%d", &value) != 1)
			{
				printf("Conversion error !");
				return 1;
			}

			points[(int)(counter / N_FEATURES)][counter % N_FEATURES] = value;
			counter++;
		}
	}

	//Send data to the input stream
	for (int i = 0; i < N_POINTS; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			intSdCh aValue;
			aValue.data = points[i * i + 2][j * j + 5];
			aValue.last = 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	//The first centroids are randomly initialised
	for (int i = 0; i < N_CLUSTER; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			intSdCh aValue;
			aValue.data = points[i][j];
			aValue.last = (i * j == (N_CLUSTER - 1) * (N_FEATURES - 1)) ? 1 : 0;
			aValue.strb = -1;
			aValue.keep = 15;
			aValue.user = 0;
			aValue.id = 0;
			aValue.dest = 0;
			inputStream.write(aValue);
		}
	}

	KMeans(inputStream, outputStream);

	printf("Result={");
	for (int i = 0; i < (N_POINTS); i++)
	{
		intSdCh valOut;
		outputStream.read(valOut);
		printf("%d", (int)valOut.data);
		if (i != N_POINTS - 1)
		{
			printf(",");
		}
	}
	printf("} \n");
	return 0;
}
