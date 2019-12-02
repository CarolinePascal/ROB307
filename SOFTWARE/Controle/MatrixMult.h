#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define DIM 32

typedef ap_axis<32, 2, 5, 6> intSdCh;

void matrixMult(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream);
