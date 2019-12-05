#include <chrono>
#include <cstdio>
#include <math.h>

#define WIDTH 32
#define HEIGHT 32
#define WIN_SIZE 3 //Must be odd !
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

inline bool bounds_ok(unsigned char y, unsigned char x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

void convolution(unsigned char picture[HEIGHT][WIDTH], unsigned char filteredPicture[HEIGHT][WIDTH])
{
    unsigned char result = 0;
    unsigned char counter = 0;

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
}

int main()
{
    //Array initialisations
    unsigned char picture[HEIGHT][WIDTH];
    unsigned char filteredPicture[HEIGHT][WIDTH];

    //Fill the picture
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            //picture[y][x] = rand() % 255;
            picture[y][x] = (x == y) ? 100 : 0;
        }
    }

    auto t1 = std::chrono::system_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        convolution(picture, filteredPicture);
    }

    auto t2 = std::chrono::system_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = duration / 1000.;

    printf("Code execution time : %f \n", ellapsed);

    //Fill the picture
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            //printf("%d,", filteredPicture[y][x]);
        }
        //printf("\n");
    }

    return 0;
}