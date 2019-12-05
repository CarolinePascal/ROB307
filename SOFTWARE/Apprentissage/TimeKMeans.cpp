#include <chrono>
#include <cstdio>
#include <math.h>

#define N_FEATURES 3
#define N_CLUSTER 3
#define N_POINTS 10

#define N_ITER 10

#define K 5

void KMeans(unsigned char points[N_POINTS][N_FEATURES], unsigned char centroids[N_CLUSTER][N_FEATURES], unsigned char results[N_POINTS])
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Centroids of the clusters - step i+1
    unsigned char new_centroids[N_CLUSTER][N_FEATURES] = {0};
    //Counter of the number of points in each cluster
    unsigned int np_cluster[N_CLUSTER] = {0};

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
}

unsigned char KNN(unsigned char points[N_POINTS][N_FEATURES], unsigned char clusters[N_POINTS], unsigned char newPoint[N_FEATURES])
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Distance tool list
    unsigned int distances[N_POINTS];
    //Counter of clusters
    unsigned int counter[N_CLUSTER] = {0};

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
    for (int i = 0; i < N_CLUSTER - 1; i++)
    {
        if (counter[i + 1] > counter[i])
        {
            cluster = i + 1;
        }
    }

    return (cluster);
}

main()
{
    unsigned char points[N_POINTS][N_FEATURES];
    unsigned char centroids[N_CLUSTER][N_FEATURES];
    unsigned char clusters[N_POINTS];
    unsigned char newPoint[N_FEATURES];

    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            points[i][j] = rand() % 255;
        }
    }

    for (int i = 0; i < N_CLUSTER; i++)
    {
        int index = rand() % (N_POINTS - 1);
        for (int j = 0; j < N_FEATURES; j++)
        {
            centroids[i][j] = points[index][j];
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        KMeans(points, centroids, clusters);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    double ellapsed = duration / 1000.;

    printf("Code execution time : %f \n", ellapsed);

    for (int i = 0; i < N_POINTS; i++)
    {
        printf("%i,", clusters[i]);
    }
    printf("\n");

    for (int i = 0; i < N_FEATURES; i++)
    {
        newPoint[i] = points[0][i];
    }

    int classPoint = 0;

    t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        classPoint = KNN(points, clusters, newPoint);
    }

    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    ellapsed = duration / 1000.;

    printf("Code execution time : %f \n", ellapsed);

    printf("%i \n", classPoint);

    return 0;
}