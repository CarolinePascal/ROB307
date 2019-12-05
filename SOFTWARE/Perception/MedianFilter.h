#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define WIDTH 64
#define HEIGHT 64
#define WIN_SIZE 3 //Must be odd !
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

typedef ap_axis<32, 2, 5, 6> intSdCh;

inline bool bounds_ok(unsigned char y, unsigned char x);
void filter(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream);
