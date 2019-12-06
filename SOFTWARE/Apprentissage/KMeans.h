
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define N_FEATURES 3
#define N_CLUSTER 3
#define N_POINTS 10

#define N_ITER 10

#define K 5

typedef ap_axis<32, 2, 5, 6> intSdCh;

/*!
 * \brief Computes the K-Means clusterisation method on a given set of points with a certain number of features each 
 * \param inStream, hls::stream<intSdCh> input stream, must send the points set and the first centroids
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the clusterisation
 */
void KMeans(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);

/*!
 * \brief [UNUSED] Computes the K-NN classification method on a given set of clusturised points withs a certain number of features each 
 * \param inStream, hls::stream<intSdCh> input stream, must send the points set, their clusters and the point to classify
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the classification
 */
void KNN(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
