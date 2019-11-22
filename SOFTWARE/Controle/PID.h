#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define LEN_STATE 5

typedef ap_axis<32, 2, 5, 6> intSdCh;

void PID(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);