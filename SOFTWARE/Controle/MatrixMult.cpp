#include "MatrixMult.h"

void matrixMult(hls::stream<intSdCh> &in_stream, hls::stream<intSdCh> &out_stream)
{
    //Array initialisations
    float A[DIM][DIM];
    float B[DIM][DIM];
    float C[DIM][DIM];

    intSdCh valRef;

READ_A:
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            intSdCh valIn = in_stream.read();
            A[i][j] = valIn.data;
            if (i * j == 0)
            {
                valRef = valIn;
            }
        }
    }

READ_B:
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            intSdCh valIn = in_stream.read();
            B[i][j] = valIn.data;
        }
    }

//Perform multiplication
LOOP1:
    for (int ia = 0; ia < DIM; ++ia)
    {
    LOOP2:
        for (int ib = 0; ib < DIM; ++ib)
        {
            float sum = 0;

        LOOP3:
            for (int id = 0; id < DIM; ++id)
            {
                sum += A[ia][id] * B[id][ib];
            }

            C[ia][ib] = sum;
        }
    }

SEND:
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            intSdCh valOut;
            valOut.data = C[i][j];
            valOut.keep = valRef.keep;
            valOut.strb = valRef.strb;
            valOut.user = valRef.user;
            valOut.last = (i * j == (DIM - 1) * (DIM - 1)) ? 1 : 0;
            valOut.id = valRef.id;
            valOut.dest = valRef.dest;
            out_stream << valOut;
        }
    }
}
