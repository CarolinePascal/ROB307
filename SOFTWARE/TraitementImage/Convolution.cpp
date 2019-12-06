#include "Convolution.h"
#include <math.h>

inline bool bounds_ok(unsigned char y, unsigned char x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

void convolution(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Picture
    unsigned char picture[HEIGHT][WIDTH];
    //FilteredPicture
    unsigned char filteredPicture[HEIGHT][WIDTH];

    intSdCh valRef;

    unsigned int result = 0;
    unsigned char counter = 0;

DATA_INY:
    for (int x = 0; x < WIDTH; x++)
    {
    DATA_INX:
        for (int y = 0; y < HEIGHT; y++)
        {
            intSdCh valIn = inStream.read();
            picture[y][x] = (unsigned char)valIn.data;
            if (x * y == 0)
            {
                valRef = valIn;
            }
        }
    }

LOOPY:
    for (int y = 0; y < HEIGHT; y++)
    {
    LOOPX:
        for (int x = 0; x < WIDTH; x++)
        {
            result = 0;
            counter = 0;
            //Load window
        LOADI:
            for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
            {
            LOADJ:
                for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
                {
                    if (bounds_ok(y + i, x + j))
                    {
                        result += picture[y + i][x + j];
                        counter += 1;
                    }
                }
            }

            filteredPicture[y][x] = (unsigned char)floor(result / counter);
        }
    }

//Send results
DATA_OUTY:
    for (int x = 0; x < WIDTH; x++)
    {
    DATA_OUTX:
        for (int y = 0; y < HEIGHT; y++)
        {
            intSdCh valOut;
            valOut.data = filteredPicture[y][x];
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (x + y == HEIGHT - 1 + WIDTH - 1) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            outStream << valOut;
        }
    }
}
