#include "hls_stream.h"
#include "Convolution.h"

#define WIN_SIZE 3 // must be odd
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

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
                result += window[i + HALF_SIZE][j + HALF_SIZE] * (i + j);
    return result;
}

// A buffered implementation of a 2D filter.
void convolution(hls::stream &out_stream, hls::stream &in_stream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    int line_buf[WIN_SIZE - 1][WIDTH];
    int window[WIN_SIZE][WIN_SIZE];
    int right[WIN_SIZE];

    // Load initial values into line buffer
    int read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;
buf_x1:
    for (int x = WIDTH - HALF_SIZE - 1; x < WIDTH; x++)
        line_buf[HALF_SIZE - 1][x] = in_stream.read();
buf_y:
    for (int y = HALF_SIZE; y < WIN_SIZE - 1; y++)
    buf_x2:
        for (int x = 0; x < WIDTH; x++)
            line_buf[y][x] = in_stream.read();

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

            // Write output value
            out_stream.write(val_out);

            // Shift line buffer column up
            right[0] = line_buf[0][x];
            for (int y = 1; y < WIN_SIZE - 1; y++)
                right[y] = line_buf[y - 1][x] = line_buf[y][x];

            // Read input value
            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in_stream.read();
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