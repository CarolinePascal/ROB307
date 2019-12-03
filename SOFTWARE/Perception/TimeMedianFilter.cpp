#include <chrono>
#include <cstdio>
#include <math.h>

#define WIDTH 32
#define HEIGHT 32
#define WIN_SIZE 3 //Must be odd !
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
    int size = 0;
    int flattenWindow[WIN_SIZE * WIN_SIZE];

FLATTEN1:
    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
    {
    FLATTEN2:
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
        {
            if (bounds_ok(y + i, x + j))
            {
                flattenWindow[size] = window[i + HALF_SIZE][j + HALF_SIZE];
                size++;
            }
        }
    }

FIRST:
    for (int i = 0; i < size - 1; i++)
    {
    SECOND:
        for (int j = 0; j < size - i - 1; j++)
        {

            if (flattenWindow[j] > flattenWindow[j + 1])
            {
                int temp = flattenWindow[j];
                flattenWindow[j] = flattenWindow[j + 1];
                flattenWindow[j + 1] = temp;
            }
        }
    }
    return (flattenWindow[(int)floor(size / 2)]);
}

/*!
 * \brief Computes the filter over all the picture
 * \param inStream, hls::stream<intSdCh> input stream, must send the picture
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the convolution
 */
void filter(int picture[HEIGHT][WIDTH], int filteredPicture[HEIGHT][WIDTH])
{
    //Array initialisation

    //Convolution window
    int window[WIN_SIZE][WIN_SIZE];

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
                    window[i + HALF_SIZE][j + HALF_SIZE] = picture[y + i][x + j];
                }
            }

            filteredPicture[y][x] = single_medianFilter(window, y, x);
        }
    }
}

main()
{
    //Array initialisations
    int picture[HEIGHT][WIDTH];
    int filteredPicture[HEIGHT][WIDTH];

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

    filter(picture, filteredPicture);

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    printf("Code execution time : %ld \n", duration);

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