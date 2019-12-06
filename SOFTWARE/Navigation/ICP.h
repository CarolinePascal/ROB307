#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define LENGTH_DAT 10
#define LENGTH_REF 10
#define N_FEATURES 2

typedef ap_axis<32, 2, 5, 6> intSdCh;

/*!
 * \brief Computes the closest-point association between two set of points with a certain number of features
 * \param inStream, hls::stream<intSdCh> input stream, must send the two sets of points
 * \param outStream, hls::stream<intSdCh> output stream, sends the result of the association
 */
void association(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
