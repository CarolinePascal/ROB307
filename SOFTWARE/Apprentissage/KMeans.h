
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define N_FEATURES 50
#define N_CLUSTER 5
#define N_POINTS 20

#define N_ITER 10

#define K 5

typedef ap_axis<32, 2, 5, 6> intSdCh;

void KMeans(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);

void KNN(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
