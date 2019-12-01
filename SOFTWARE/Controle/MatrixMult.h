#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define SIZE 32

void matrixMult(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream);