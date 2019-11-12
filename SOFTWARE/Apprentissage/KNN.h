
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define N_FEATURES 50
#define N_CLUSTER 20
#define N_POINTS 500

typedef ap_axis<32,2,5,6> intSdCh;

void KNN(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
int get_cluster(volatile float data_set[N_FEATURES], volatile float centroids[N_CLUSTER][N_FEATURES]);
