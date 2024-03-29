#include "KMeans.h"

void KMeans(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Points to be clusterised
    unsigned char points[N_POINTS][N_FEATURES];
    //Centroids of the clusters - step i
    unsigned char centroids[N_CLUSTER][N_FEATURES];
    //Centroids of the clusters - step i+1
    unsigned char new_centroids[N_CLUSTER][N_FEATURES] = {0};
    //Counter of the number of points in each cluster
    unsigned int np_cluster[N_CLUSTER] = {0};
    //Indices of the corresponding cluster for each point
    unsigned char results[N_POINTS];

    intSdCh valRef;

READ_POINTS:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            points[i][j] = (unsigned char)valIn.data;
            if (i + j == 0)
            {
                valRef = valIn;
            }
        }
    }

READ_CENTROIDS:
    for (int i = 0; i < N_CLUSTER; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            centroids[i][j] = (unsigned char)valIn.data;
        }
    }

    unsigned char end = 0;
    unsigned int distance, min_distance;
    unsigned char cluster;

    //Limit the number of iterations
ITERATIONS:
    for (int n = 0; n < N_ITER; n++)
    {
        end = 1;

        //Compute the asssociations between points and centroids into clusters

    CLUSTER:
        //For each point...
        for (int i = 0; i < N_POINTS; i++)
        {
            min_distance = 999999999;
        //...Compute the distance to centroids
        GET_CLUSTER1:
            for (int c = 0; c < N_CLUSTER; c++)
            {
                distance = 0;

            GET_CLUSTER2:
                for (int j = 0; j < N_FEATURES; j++)
                {
                    distance += ((points[i][j] - centroids[c][j]) * (points[i][j] - centroids[c][j]));
                }

                //If the distance is smaller, update it
                if (distance < min_distance)
                {
                    min_distance = distance;
                    cluster = c;
                }
            }

            //Check wether the associations changed
            if (cluster != results[i])
            {
                results[i] = cluster;
                end = 0;
            }
            np_cluster[cluster] += 1;
        }

        //If the associations didn't change, exit the loop
        if (end == 1)
        {
            break;
        }

        //Otherwise, uptade the centroids to their new values
    SUM:
        for (int i = 0; i < N_POINTS; i++)
        {
            for (int j = 0; j < N_FEATURES; j++)
            {
                new_centroids[results[i]][j] += points[i][j];
            }
        }

    UPDATE:
        for (int c = 0; c < N_CLUSTER; c++)
        {
            for (int f = 0; f < N_FEATURES; f++)
            {
                if (np_cluster[c] != 0)
                {
                    centroids[c][f] = (unsigned char)(new_centroids[c][f] / np_cluster[c]);
                }
                new_centroids[c][f] = 0;
            }
            np_cluster[c] = 0;
        }
    }

SEND:
    for (int i = 0; i < N_POINTS; i++)
    {
        intSdCh valOut;
        valOut.data = results[i];
        valOut.keep = valRef.keep;
        valOut.strb = valRef.strb;
        valOut.user = valRef.user;
        valOut.last = (i == N_POINTS - 1) ? 1 : 0;
        valOut.id = valRef.id;
        valOut.dest = valRef.dest;
        outStream << valOut;
    }
}

void KNN(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Clusterised points
    unsigned char points[N_POINTS][N_FEATURES];
    //Clusters of each point in points
    unsigned char clusters[N_POINTS];
    //New point to classify
    unsigned char newPoint[N_FEATURES];
    //Distance tool list
    unsigned int distances[N_POINTS];
    //Counter of clusters
    unsigned int counter[N_CLUSTER] = {0};

    intSdCh valRef;

READ_POINTS:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            points[i][j] = (unsigned char)valIn.data;
            if (i == 0 && j == 0)
            {
                valRef = valIn;
            }
        }
    }

READ_CLUSTERS:
    for (int i = 0; i < N_POINTS; i++)
    {
        intSdCh valIn = inStream.read();
        clusters[i] = (unsigned char)valIn.data;
    }

READ_NEWPOINT:
    for (int i = 0; i < N_FEATURES; i++)
    {
        intSdCh valIn = inStream.read();
        newPoint[i] = (unsigned char)valIn.data;
    }

    unsigned int distance;

    //Compute the distances from the new point to the other points
GET_DIST1:
    for (int i = 0; i < N_POINTS; i++)
    {
        distance = 0;

    GET_DIST2:
        for (int j = 0; j < N_FEATURES; j++)
        {
            distance += ((newPoint[j] - points[i][j]) * (newPoint[j] - points[i][j]));
        }

        distances[i] = distance;
    }

    unsigned int temp;

    //Sort the distances
SORT1:
    for (int i = 0; i < N_POINTS - 1; i++)
    {
    SORT2:
        for (int j = 0; j < N_POINTS - i - 1; j++)
        {
            if (distances[j] > distances[j + 1])
            {
                temp = clusters[i];
                clusters[i] = clusters[i + 1];
                clusters[i + 1] = temp;

                temp = distances[i];
                distances[i] = distances[i + 1];
                distances[i + 1] = temp;
            }
        }
    }

    //Keep only the K closest neighbours
COUNTER:
    for (int i = 0; i < K; i++)
    {
        counter[clusters[i]] += 1;
    }

    unsigned char cluster = 0;

    //Classification
GET_CLASS:
    for (int i = 0; i < N_CLUSTER; i++)
    {
        if (counter[i + 1] > counter[i])
        {
            cluster = i + 1;
        }
    }

    //Send the result
    intSdCh valOut;
    valOut.data = cluster;
    valOut.keep = valRef.keep;
    valOut.strb = valRef.strb;
    valOut.user = valRef.user;
    valOut.last = 1;
    valOut.id = valRef.id;
    valOut.dest = valRef.dest;
    outStream << valOut;
}
