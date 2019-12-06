#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define WIDTH 64
#define HEIGHT 64
#define WIN_SIZE 3 //Must be odd !
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

typedef ap_axis<32, 2, 5, 6> intSdCh;
/*!
 * \brief Asserts wether a certain pixel is within the picture boundries
 * \param x unsigned char, column of the pixel
 * \param y unsigned char, row of the pixel
 * \return True if the pixel is in the picture, False otherwise
 */
inline bool bounds_ok(unsigned char y, unsigned char x);

/*!
 * \brief Computes the convolution over all the picture
 * \param inStream, hls::stream<intSdCh> input stream, must send the picture
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the convolution
 */
void convolution(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);
