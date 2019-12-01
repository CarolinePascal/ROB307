#include "KMeans.h"

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

    //Array initialisation

    //Points to be clusterised
    int points[N_POINTS][N_FEATURES];
    //Centroids of the clusters - step i
    int centroids[N_CLUSTER][N_FEATURES];
    //Centroids of the clusters - step i+1
    int new_centroids[N_CLUSTER][N_FEATURES];
    //Counter of the number of points in each cluster
    int np_cluster[N_CLUSTER];
    //Indices of the corresponding cluster for each point
    int results[N_POINTS];

    intSdCh valRef;

    READ_POINTS:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            points[i][j] = valIn.data;
            if (i == 0 && j == 0)
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
            centroids[i][j] = valIn.data;
        }
    }

    int end = 0;

    ITERATIONS:
    for(int n = 0; n < N_ITER; n++)
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
        if(end == 1)
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
                centroids[c][f] = (int)(new_centroids[c][f] / np_cluster[c]);
                new_centroids[c][f] = 0;
            }
            np_cluster[c] = 0;
        }
    }

    SEND:
    for (int idx = 0; idx < N_POINTS; idx++)
    {
        intSdCh valOut;
        valOut.data = results[idx];
        valOut.keep = valRef.keep;
        valOut.strb = valRef.strb;
        valOut.user = valRef.user;
        valOut.last = (idx == (N_POINTS - 1)) ? 1 : 0;
        valOut.id = valRef.id;
        valOut.dest = valRef.dest;
        outStream << valOut;
    }  
}

/*!
 * \brief Computes the K-NN classification method on a given set of clusturised points containing a certain number of features each 
 * \param inStream, hls::stream<intSdCh> input stream, must send the points set, their clusters and the point to classify
 * \param outStream, hls::stream<intSdCh> output stream, returns the result of the classification
 */
void KNN(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream)
{
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS

    //Array initialisation

    //Clusterised points
    int points[N_POINTS][N_FEATURES];
    //Clusters of the points
    int clusters[N_POINTS];
    //Point to classify
    int newPoint[N_FEATURES];

    READ_POINTS:
    for (int i = 0; i < N_POINTS; i++)
    {
        for (int j = 0; j < N_FEATURES; j++)
        {
            intSdCh valIn = inStream.read();
            points[i][j] = valIn.data;
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
            clusters[i] = valIn.data;
    }

    READ_NEWPOINT:
    for(int i = 0; i < N_FEATURES; i++)
    {
        intSdCh valIn = inStream.read();
        newPoint[i] = valIn.data;
    }

    //Get the cluster corresponding to the new point according to the clusters of its K nearest neighbours
    int cluster = get_class(newPoint,points,clusters);

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

/*!
 * \brief Computes the cluster associated to a certain point given a list of clusterised points
 * \param newPoint int[N_FEATURES], point to be associated
 * \param points int[N_POINTS][N_FEATURES], clusterised points
 * \param clusters int[N_POINTS], index of the cluster of each element of points
 * \return int, index of the associated cluster
 */
int get_class(int newPoint[N_FEATURES], int points[N_POINTS][N_FEATURES], int clusters[N_POINTS])
{
    float distances[N_POINTS];
    int counter[N_CLUSTER];

    float distance = 0;
    float min_distance = -99999999999;
    int index = -1;

    //Computing the distances
    GET_DIST1:
    for(int i = 0; i < N_POINTS; i++)
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
    for(int i = 0; i < N_POINTS - 1; i++)
    {
        SORT2:
        for(int j=0; j < N_POINTS - i - 1; j++)
        {
            if(distances[j] > distances[j+1])
            {
                temp = clusters[i];
                clusters[i] = clusters[i+1];
                clusters[i+1] = temp;

                temp = distances[i];
                distances[i] = distances[i+1];
                distances[i+1] = temp;


            }
        } 
    }

    //We only keep the K closest neighbours
    COUNTER:
    for(int i = 0; i < K; i++)
    {
        counter[clusters[i]] += 1;
    }

    cluster = 0;
    
    //Classification
    GET_CLASS:
    for(int i = 0; i < N_CLUSTER; i++)
    {
        if(counter[i+1]>counter[i])
        {
            cluster = i+1;
        }
    }

    return(cluster);
}

/*!
 * \brief Computes the cluster associated to a certain point given the centroids of the clusters
 * \param point int[N_FEATURES], point to be associated
 * \param centrois int[N_CLUSTER][N_FEATURES], centroids of the clusters
 * \return int, index of the associated centroid
 */
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
