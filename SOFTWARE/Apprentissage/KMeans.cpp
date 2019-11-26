#include "KMeans.h"
#include <math.h>

//Initialise arrays

//Points to be clusterised
volatile int points[N_POINTS][N_FEATURES];
//Centroids of the clusters - step i
volatile int centroids[N_CLUSTER][N_FEATURES];
//Centroids of the clusters - step i+1
volatile int new_centroids[N_CLUSTER][N_FEATURES];
//Counter of the number of points in each cluster
volatile int np_cluster[N_CLUSTER];
//Indices of the corresponding cluster for each point
volatile int results[N_POINTS];

intSdCh valref;

/*!
 * \brief Computes the K-Means clusterisation method on a given set of points containing a certain number of features each 
 * \param inStream, hls::stream<intSdCh> input stream, must send the points set and the first centroids
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the clusterisation
 */
void KMeans(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    int end = 1;

READ_POINTS:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            points[i][j] = valIn.data;
            if (i == 0 && j == 0)
            {
                valref = valIn;
            }
        }
    }

READ_CENTROIDS:
    for (int i = 0; i < N_CLUSTER; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            centroids[i][j] = valIn.data;
        }
    }

CLUSTER:
    for (int i = 0; i < N_POINTS; i++)
    {
        int cluster = get_cluster(points[i], centroids);
        if (cluster != results[i])
        {
            results[i] = cluster;
            end = 0;
        }
        np_cluster[cluster] += 1;
    }

SUM:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            new_centroids[results[i]][j] += points[i][j];
        }
    }

    if (end == 0)
    {
    UPDATE:
        for (int c = 0; c < N_CLUSTER; c++)
        {
            for (int f = 0; f < N_FEATURES; f++)
            {
                centroids[c][f] = (int)(new_centroids[c][f] / np_cluster[c]);
                new_centroids[c][f] = 0;
            }
            np_cluster[c] = 0;
        }
    }

    //Send results
SEND:
    for (int idx = 0; idx < N_POINTS; idx++)
    {
        intSdCh valOut;
        valOut.data = results[idx];
        valOut.keep = valref.keep;
        valOut.strb = valref.strb;
        valOut.user = valref.user;
        valOut.last = (idx == (N_POINTS - 1)) ? 1 : 0;
        valOut.id = valref.id;
        valOut.dest = valref.dest;
        outStream << valOut;
    }
}

/*!
 * \brief Computes the cluster associated to a certain point given the centroids
 * \param point int[N_FEATURES], point to be associated
 * \param centrois int[N_CLUSTER][N_FEATURES], centroids
 * \return int, the index of the associated centroid
 */
int get_cluster(volatile int point[N_FEATURES], volatile int centroids[N_CLUSTER][N_FEATURES])
{
    float distance = 0;
    float min_distance = 99999999999;
    int cluster = 0;

get_cluster_label2:
    for (int c = 0; c < N_CLUSTER; c++)
    {
        distance = 0;

    get_cluster_label1:
        for (int j = 0; j < N_FEATURES; j++)
        {
            distance += ((point[j] - centroids[c][j]) * (point[j] - centroids[c][j]));
        }
        distance = sqrt(distance);
        if (distance < min_distance)
        {
            min_distance = distance;
            cluster = c;
        }
    }
    return cluster;
}
