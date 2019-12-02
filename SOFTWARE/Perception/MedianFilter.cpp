#include "hls_stream.h"
#include "MedianFilter.h"

#define WIN_SIZE 3 // must be odd
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

/*!
 * \brief Asserts wether a certain pixel is within the picture boundries
 * \param x int, column of the pixel
 * \param y int, row of the pixel
 * \return True if the pixel is in the picture, False otherwise
 */
inline bool bounds_ok(int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

/*!
 * \brief Computes the median filter at a certain pixel of the picture
 * \param windows int[WIN_SIZE][WIN_SIZE], the filtering window around the considered pixel
 * \param x int, column of the pixel
 * \param y int, row of the pixel
 * \return int, result of the filter
 */
inline int single_medianFilter(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
FIRST:
    for (int i = 0; i < WIN_SIZE * WIN_SIZE - 1; i++)
    {
    SECOND:
        for (int j = 0; j < WIN_SIZE * WIN_SIZE - i - 1; j++)
        {
            if (window[(int)floor(j / WIN_SIZE)][j % WIN_SIZE] > window[(int)floor((j + 1) / WIN_SIZE)][(j + 1) % WIN_SIZE])
            {
                int temp = window[(int)floor(j / WIN_SIZE)][j % WIN_SIZE];
                window[(int)floor(j / WIN_SIZE)][j % WIN_SIZE] = window[(int)floor((j + 1) / WIN_SIZE)][(j + 1) % WIN_SIZE];
                window[(int)floor((j + 1) / WIN_SIZE)][(j + 1) % WIN_SIZE] = temp;
            }
        }
    }
    return (window[(int)floor(WIN_SIZE / 2)][(int)floor(WIN_SIZE / 2)]);
}

/*!
 * \brief Computes the filter over all the picture
 * \param inStream, hls::stream<intSdCh> input stream, must send the picture
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the convolution
 */
void filter(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Picture
    int picture[WIDTH][HEIGHT];
    //Convolution window
    int window[WIN_SIZE][WIN_SIZE];

    intSdCh valRef;

DATA_INY:
    for (int i = 0; i < WIDTH; i++)
    {
    DATA_INX:
        for (int j = 0; j < HEIGHT; j++)
        {
            intSdCh valIn = in_stream.read();
            picture[i][j] = valIn.data;
            if (i * j == 0)
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
            //Load window
        LOADI:
            for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
            {
            LOADJ:
                for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
                {
                    if (bounds_ok(y + i, x + j))
                    {
                        window[i + HALF_SIZE][j + HALF_SIZE] = picture[y + i][x + j];
                    }
                }
            }

            //Send results
            intSdCh valOut;
            valOut.data = single_medianFilter(window, y, x);
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (x * y == (HEIGHT - 1) * (WIDTH - 1)) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            out_stream << valOut;
        }
    }
}

/*!
 * \brief Computes the filter over all the picture with buffer
 * \param inStream, hls::stream<intSdCh> input stream, must send the picture
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the convolution
 */
void filterBuffer(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Arrays initialisation

    //Convolution window
    int window[WIN_SIZE][WIN_SIZE];
    //Line buffer
    int line_buf[WIN_SIZE - 1][WIDTH];
    //Column buffer
    int right[WIN_SIZE];

    int val_in = 0;
    intSdCh valRef;

    // Load initial values into line buffer
    int read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;

BUFFERX1:
    for (int x = WIDTH - HALF_SIZE - 1; x < WIDTH; x++)
    {
        intSdCh valIn = in_stream.read();
        line_buf[HALF_SIZE - 1][x] = valIn.data;
        if (x == 0)
        {
            valRef = valIn;
        }
    }

BUFFERY:
    for (int y = HALF_SIZE; y < WIN_SIZE - 1; y++)
    BUFFERX2:
        for (int x = 0; x < WIDTH; x++)
        {
            intSdCh valIn = in_stream.read();
            line_buf[y][x] = valIn.data;
        }

//Copy the values into the convolution window
WINDOWY:
    for (int y = HALF_SIZE; y < WIN_SIZE; y++)
    WINDOWX:
        for (int x = HALF_SIZE; x < WIN_SIZE; x++)
            window[y][x] = line_buf[y - 1][x + WIDTH - WIN_SIZE];

//Start convolution
LOOPY:
    for (int y = 0; y < HEIGHT; y++)
    {
    LOOPX:
        for (int x = 0; x < WIDTH; x++)
        {
            //Send results
            intSdCh valOut;
            valOut.data = single_medianFilter(window, y, x);
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (x * y == (HEIGHT - 1) * (WIDTH - 1)) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            out_stream << valOut;

            //Shift line buffer column up
            right[0] = line_buf[0][x];
        SHIFT_UP:
            for (int y = 1; y < WIN_SIZE - 1; y++)
            {
                right[y] = line_buf[y - 1][x] = line_buf[y][x];
            }

            // Read input value
            if (read_count < HEIGHT * WIDTH)
            {
                intSdCh valIn = in_stream.read();
                val_in = valIn.data;
                read_count++;
            }
            right[WIN_SIZE - 1] = line_buf[WIN_SIZE - 2][x] = val_in;

        //Shift window left
        SHIFT_LEFTY:
            for (int y = 0; y < WIN_SIZE; y++)
            {
            SHIFT_LEFTX:
                for (int x = 0; x < WIN_SIZE - 1; x++)
                {
                    window[y][x] = window[y][x + 1];
                }
            }

        //Update rightmost window values
        UPDATE:
            for (int y = 0; y < WIN_SIZE; y++)
                window[y][WIN_SIZE - 1] = right[y];
        }
    }
}
