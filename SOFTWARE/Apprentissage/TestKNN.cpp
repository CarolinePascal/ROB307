#include <stdio.h>
#include <stdlib.h>
#include "KNN.h"
#include <iostream>
#include <fstream>

#define N_POINTS 160
#define N_digits 8
#define CHUNK 16384

static char FILE_NAME[32] = "TEST.csv";
int res[N_POINTS];

void read_dataset(float** data_set);
void show_dataset(float** data, int row);
void cluster_index(int index[N_CLUSTER]);
void send_strvalue(hls::stream<intSdCh> &outStream, unsigned int to_send, ap_uint<1> last);
float get_strvalue(hls::stream<intSdCh> &inStream);

int main(){

	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

	int in_index[N_CLUSTER];
	/*************** load data *****************/
	int** data_set = new int*[N_POINTS];
	for(int i=0; i<N_POINTS; i++){
		data_set[i] = new int[N_FEATURES];
	}

	int** centroid = new int*[N_CLUSTER];
	for(int i=0; i<N_CLUSTER; i++){
		centroid[i] = new int[N_FEATURES];
	}

	for(int i=0; i<N_POINTS ; i++){
		res[i] = 0;
	}
	//int res[]={12,7,8,0,17,18,1,9,15,6,10,14,5,4,19,3,13,2,16,11} ;
	read_dataset(data_set);
	//show_dataset(data_set, N_POINTS);

	/************** send data ******************/
	for(int i=0; i<N_POINTS_IP; i++){
		for(int j=0 ; j< N_FEATURES ; j++){
			send_strvalue(inputStream, data_set[i][j] , 0);
		}
	}

	for(int i=0 ; i<N_POINTS_IP ; i++){
		intSdCh aValue;
		aValue.data = res[i];
		aValue.last = (i==(N_POINTS))? 1:0;
		aValue.strb = -1;
		aValue.keep = 15;
		aValue.user = 0;
		aValue.id = 0;
		aValue.dest = 0;
		inputStream.write(aValue);
	}

	KNN(inputStream, outputStream);

	printf("res={");
	for(int i=0 ; i<(N_POINTS_IP) ; i++){
		intSdCh valOut;
		outputStream.read(valOut);
		printf("%d", (int) valOut.data);
		if(i != N_POINTS_IP-1){
			printf(",");
		}
	}
	printf("} \n");
	for(int i=0 ; i<2 ; i++){
//		intSdCh valOut;
//		outputStream.read(valOut);
//		printf("end : %d ,", (int) valOut.data);
	}
	return 0;

}



void read_dataset(float** data_set)
{
  FILE *file;
  size_t nread;
  int feature = 0;
  int point = 0;
  char number[N_digits];
  int index = 0;
  char * buf;

  file = fopen(FILE_NAME, "r");
  buf = (char*) malloc (sizeof(char)*CHUNK);
  if (file) {
    while(nread = fread(buf, 1, CHUNK, file)>0){
      for(int j = 0; j < CHUNK; j++){
        if(buf[j]==','){
          data_set[point][feature] = (int)atof(number);
          index = 0;
          if(feature < N_FEATURES-1){
            feature++;}
          else{
            break;}
        }else{
          if(buf[j] == (char)0x0a){
            data_set[point][feature] = (int)atof(number);
            feature = 0;
            index = 0;
            if(point < N_POINTS-1){
              point++;}
            else{break;}
          }else{
            if(index<N_digits){
              number[index] = buf[j];
              index++;
            }
          }
        }
      }
    }
      fclose(file);
  }
  free (buf);
}

void show_dataset(float** data, int row)
{
  for(int i = 0; i <row ; i++){
    for(int j=0 ; j < N_FEATURES ; j++){
      std::cout<<data[i][j]<<" , ";
    }
    std::cout<<std::endl;
  }
  std::cout<<"the data \n";
}


void send_strvalue(hls::stream<intSdCh> &outStream, unsigned int to_send, ap_uint<1> last){
	intSdCh aValue;
	aValue.data = to_send;
	aValue.last = last;
	aValue.strb = -1;
	aValue.keep = 15;
	aValue.user = 0;
	aValue.id = 0;
	aValue.dest = 0;
	outStream.write(aValue);
}

float get_strvalue(hls::stream<intSdCh> &inStream){
	intSdCh receive=inStream.read();
	return receive.data;
}

