#include "MedianFilter.h"
#include <math.h>

inline bool bounds_ok(unsigned char y, unsigned char x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

void filter(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Picture
    unsigned char picture[HEIGHT][WIDTH];
    //FilteredPicture
    unsigned char filteredPicture[HEIGHT][WIDTH];
    //Convolution window
    unsigned char window[WIN_SIZE * WIN_SIZE];

    intSdCh valRef;

    unsigned char temp1, temp2, result;
    unsigned int counter, index;

DATA_INY:
    for (int x = 0; x < WIDTH; x++)
    {
    DATA_INX:
        for (int y = 0; y < HEIGHT; y++)
        {
            intSdCh valIn = inStream.read();
            picture[y][x] = (unsigned char)valIn.data;
            if (x + y == 0)
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
                        index = counter;
                        result = picture[y + i][x + j];
                        SORT1:
                            for (int k = 0; k < WIN_SIZE * WIN_SIZE; k++)
                            {
                                if (k >= counter)
                                {
                                    break;
                                }

                                if (window[k] > result)
                                {
                                    temp1 = window[k];
                                    index = k;
                                SORT2:
                                    for (int l = 0; l <= WIN_SIZE * WIN_SIZE; l++)
                                    {
                                        if (l < k + 1)
                                        {
                                            continue;
                                        }

                                        else
                                        {
                                            if (l > counter)
                                            {
                                                break;
                                            }
                                            temp2 = window[l];
                                            window[l] = temp1;
                                            temp1 = temp2;
                                        }
                                    }
                                    break;
                            }
                        }
                        window[index] = result;
                        counter += 1;
                    }
                }
            }
            filteredPicture[y][x] = window[(int)floor(counter / 2)];
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
