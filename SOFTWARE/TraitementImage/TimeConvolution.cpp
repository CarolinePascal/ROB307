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
 * \brief Computes the convolution operation at a certain pixel of the picture
 * \param windows int[WIN_SIZE][WIN_SIZE], the convolution window around the considered pixel
 * \param x int, column of the pixel
 * \param y int, row of the pixel
 * \return int, result of the convolution
 */
inline int single_convolution(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;
    int counter = 0;
ROWS:
    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
    {
    COLUMNS:
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
        {
            if (bounds_ok(y + i, x + j))
            {
                result += window[i + HALF_SIZE][j + HALF_SIZE];
                counter += 1;
            }
        }
    }
    return (int)floor(result / counter);
}

void convolution(int picture[HEIGHT][WIDTH], int filteredPicture[HEIGHT][WIDTH])
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

            filteredPicture[y][x] = single_convolution(window, y, x);
        }
    }
}

/*
void convolutionBuffer(int picture[HEIGHT][WIDTH],int filteredPicture[HEIGHT][WIDTH])
{
    //Arrays initialisation

    //Convolution window
    int window[WIN_SIZE][WIN_SIZE];
    //Line buffer
    int line_buf[WIN_SIZE - 1][WIDTH];
    //Column buffer
    int right[WIN_SIZE];

    // Load initial values into line buffer
    int read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;

    int counter = 0;

    auto t1 = std::chrono::high_resolution_clock::now();

BUFFERX1:
    for (int x = WIDTH - HALF_SIZE - 1; x < WIDTH; x++)
    {
        line_buf[HALF_SIZE - 1][x] = picture[counter%WIDTH][(int)floor(counter/WIDTH)];
        counter++;
    }

BUFFERY:
    for (int y = HALF_SIZE; y < WIN_SIZE - 1; y++)
    BUFFERX2:
        for (int x = 0; x < WIDTH; x++)
        {
            line_buf[y][x] = picture[counter%WIDTH][(int)floor(counter/WIDTH)];
            counter++;
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
            filteredPicture[y][x] = single_convolution(window,y,x);

            //Shift line buffer column up
            right[0] = line_buf[0][x];
        SHIFT_UP:
            for (int y = 1; y < WIN_SIZE - 1; y++)
            {
                right[y] = line_buf[y - 1][x] = line_buf[y][x];
            }

            int val_in = 0;
            // Read input value
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = picture[counter%WIDTH][(int)floor(counter/WIDTH)];
                counter++;
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

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    printf("Code execution time : %ld \n", duration);
}
*/

int main()
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

    convolution(picture, filteredPicture);

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