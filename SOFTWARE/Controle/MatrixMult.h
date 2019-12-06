#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define DIM 32

typedef ap_axis<32, 2, 5, 6> intSdCh;

/*!
 * \brief Computes the multiplication of two matrices
 * \param inStream, hls::stream<intSdCh> input stream, must send the two matrices to be multiplied
 * \param outStream, hls::stream<intSdCh> output stream, sends the result of the multiplication
 */
void matrixMult(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
