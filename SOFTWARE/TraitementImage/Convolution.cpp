#include "hls_stream.h"
#include "Convolution.h"

inline bool bounds_ok(int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

// Defines the actual calculation for one output value.
inline int single_operation(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;
win_i:
    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
    win_j:
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
            if (bounds_ok(y + i, x + j))
                result += window[i + HALF_SIZE][j + HALF_SIZE];
    return result/(WIN_SIZE*WIN_SIZE);
}

// A buffered implementation of a 2D filter.
void convolution(hls::stream<intSdCh> &out_stream, hls::stream<intSdCh> &in_stream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    int line_buf[WIN_SIZE - 1][WIDTH];
    int window[WIN_SIZE][WIN_SIZE];
    int right[WIN_SIZE];

    int val_in = 0;
    intSdCh valRef;

    // Load initial values into line buffer
    int read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;
buf_x1:
    for (int x = WIDTH - HALF_SIZE - 1; x < WIDTH; x++)
    {
        intSdCh valIn = in_stream.read();
        val_in = valIn.data;
        line_buf[HALF_SIZE - 1][x] = val_in;
        if (x == 0)
        {
            valRef = valIn;
        }
    }
buf_y:
    for (int y = HALF_SIZE; y < WIN_SIZE - 1; y++)
    buf_x2:
        for (int x = 0; x < WIDTH; x++)
        {
            intSdCh valIn = in_stream.read();
            val_in = valIn.data;
            line_buf[y][x] = val_in;
        }

// Copy initial values into window
win_y:
    for (int y = HALF_SIZE; y < WIN_SIZE; y++)
    win_x:
        for (int x = HALF_SIZE; x < WIN_SIZE; x++)
            window[y][x] = line_buf[y - 1][x + WIDTH - WIN_SIZE];

// Start convolution
for_y:
    for (int y = 0; y < HEIGHT; y++)
    {
    for_x:
        for (int x = 0; x < WIDTH; x++)
        {

            // Calculate output value
            int val_out = single_operation(window, y, x);

            intSdCh valOut;
            valOut.data = val_out;
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (x * y == (HEIGHT - 1) * (WIDTH - 1)) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            out_stream << valOut;
            // Write output value
            //out_stream.write(val_out);

            // Shift line buffer column up
            right[0] = line_buf[0][x];
            for (int y = 1; y < WIN_SIZE - 1; y++)
                right[y] = line_buf[y - 1][x] = line_buf[y][x];

            // Read input value

            if (read_count < HEIGHT * WIDTH)
            {
                intSdCh valIn = in_stream.read();
                val_in = valIn.data;
                read_count++;
            }
            right[WIN_SIZE - 1] = line_buf[WIN_SIZE - 2][x] = val_in;

            // Shift window left
            for (int y = 0; y < WIN_SIZE; y++)
                for (int x = 0; x < WIN_SIZE - 1; x++)
                    window[y][x] = window[y][x + 1];

            // Update rightmost window values
            for (int y = 0; y < WIN_SIZE; y++)
                window[y][WIN_SIZE - 1] = right[y];
        }
    }
}
