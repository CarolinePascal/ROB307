#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define LENGTH_DAT 10
#define LENGTH_REF 10
#define N_FEATURES 2

typedef ap_axis<32, 2, 5, 6> intSdCh;

void association(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
