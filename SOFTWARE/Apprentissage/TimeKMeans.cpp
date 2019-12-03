#include <chrono>
#include <cstdio>
#include <math.h>

#define N_FEATURES 50
#define N_CLUSTER 5
#define N_POINTS 20

#define N_ITER 10

#define K 5


int get_cluster(int point[N_FEATURES], int centroids[N_CLUSTER][N_FEATURES])
{
    float distance = 0;
    float min_distance = 99999999999;
    int cluster = 0;

GET_CLUSTER1:
    for (int c = 0; c < N_CLUSTER; c++)
    {
        distance = 0;

    //Computing the distance to centroids
    GET_CLUSTER2:
        for (int j = 0; j < N_FEATURES; j++)
        {
            distance += ((point[j] - centroids[c][j]) * (point[j] - centroids[c][j]));
        }

        //If the distance is smaller, we update it
        if (distance < min_distance)
        {
            min_distance = distance;
            cluster = c;
        }
    }
    return cluster;
}

void KMeans(int points[N_POINTS][N_FEATURES], int centroids[N_CLUSTER][N_FEATURES], int results[N_POINTS])
{
    //Array initialisation

    //Centroids of the clusters - step i+1
    int new_centroids[N_CLUSTER][N_FEATURES] = {0};
    //Counter of the number of points in each cluster
    int np_cluster[N_CLUSTER] = {0};

    int end = 0;

ITERATIONS:
    for (int n = 0; n < N_ITER; n++)
    {
        end = 1;

    //Compute the asssociations between points and centroids into clusters
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

        //If the associations don't change, we exit the loop
        if (end == 1)
        {
            break;
        }

    //Otherwise, we uptade the centroids to their new values
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
                    centroids[c][f] = (int)(new_centroids[c][f] / np_cluster[c]);
                }
                new_centroids[c][f] = 0;
            }
            np_cluster[c] = 0;
        }
    }
}

int get_class(int newPoint[N_FEATURES], int points[N_POINTS][N_FEATURES], int clusters[N_POINTS])
{
    float distances[N_POINTS];
    int counter[N_CLUSTER] = {0};

    float distance = 0;
    float min_distance = -99999999999;
    int index = -1;

    //Computing the distances
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

    float temp;

//Sorting the distances
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

//We only keep the K closest neighbours
COUNTER:
    for (int i = 0; i < K; i++)
    {
        counter[clusters[i]] += 1;
    }

    int cluster = 0;

//Classification
GET_CLASS:
    for (int i = 0; i < N_CLUSTER; i++)
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
    int points[N_POINTS][N_FEATURES];
    int centroids[N_CLUSTER][N_FEATURES];
	int clusters[N_POINTS];
    int newPoint[N_FEATURES];

    for (int i = 0; i < N_POINTS; i++)
	{
		for (int j = 0; j < N_FEATURES; j++)
		{
			points[i][j] = rand()%255;
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

    KMeans(points,centroids,clusters);

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    printf("Code execution time : %ld \n", duration);

    for (int i = 0; i < N_POINTS; i++)
    {
        printf("%d,",clusters[i]);
    }
    printf("\n");

    for (int i = 0; i < N_FEATURES; i++)
    {
        newPoint[i] = points[0][i];
    }

    t1 = std::chrono::high_resolution_clock::now();

    int classPoint = get_class(newPoint,points,clusters);

    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("Code execution time : %ld \n", duration);

    printf("%d \n",classPoint);

    return 0;
}