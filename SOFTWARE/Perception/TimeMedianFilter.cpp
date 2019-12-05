#include <chrono>
#include <cstdio>
#include <math.h>

#define WIDTH 64
#define HEIGHT 64
#define WIN_SIZE 3 //Must be odd !
#define HALF_SIZE (((WIN_SIZE)-1) / 2)

inline bool bounds_ok(unsigned char y, unsigned char x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}

void filter(unsigned char picture[HEIGHT][WIDTH], unsigned char filteredPicture[HEIGHT][WIDTH])
{

    //Array initialisation

    //Convolution window
    unsigned char window[WIN_SIZE * WIN_SIZE];

    unsigned char temp1, temp2, result;
    unsigned int counter, index;

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
                        for (int k = 0; k < counter; k++)
                        {
                            if (window[k] > result)
                            {
                                temp1 = window[k];
                                index = k;
                            SORT2:
                                for (int l = k + 1; l <= counter; l++)
                                {
                                    temp2 = window[l];
                                    window[l] = temp1;
                                    temp1 = temp2;
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
}

main()
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

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        filter(picture, filteredPicture);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = duration / 1000.;

    printf("Code execution time : %f \n", ellapsed);

    //Fill the picture
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            printf("%d,", filteredPicture[y][x]);
        }
        printf("\n");
    }

    return 0;
}