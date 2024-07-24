#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 1000
#define INF INT_MAX

typedef struct
{
    int target;
    int weights[10];
} Edge;

typedef struct
{
    Edge edges[MAX_VERTICES];
    int edge_count;
} Vertex;

Vertex graph[MAX_VERTICES];
int V, N;

typedef struct
{
    int vertex;
    int distance;
    int step;
} MinHeapNode;

typedef struct
{
    MinHeapNode *array;
    int size;
    int capacity;
    int *pos;
} MinHeap;

MinHeap* createMinHeap(int capacity)
{
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode*)malloc(capacity * sizeof(MinHeapNode));

    return(minHeap);
}

void swapMinHeapNode(MinHeapNode* a, MinHeapNode* b)
{
    MinHeapNode c = *a;
    *a = *b;
    *b = c;
}

void minHeapify(MinHeap* minHeap, int ind)
{
    int smallest, left, right;
    smallest = ind;
    left = 2 * ind + 1;
    right = 2 * ind + 2;

    if (left < minHeap->size && minHeap->array[left].distance < minHeap->array[smallest].distance)
    {
        smallest = left;
    }
    
    if (right < minHeap->size && minHeap->array[right].distance < minHeap->array[smallest].distance)
    {
        smallest = right;
    }

    if (smallest != ind)
    {
        MinHeapNode smallestNode = minHeap->array[smallest];
        MinHeapNode indNode = minHeap->array[ind];

        minHeap->pos[smallestNode.vertex] = ind;
        minHeap->pos[indNode.vertex] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[ind]);

        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap)
{
    return(minHeap->size == 0);
}

MinHeapNode extractMin(MinHeap* minHeap)
{
    if (isEmpty(minHeap))
    {
        return((MinHeapNode){-1, INF, -1});
    }

    MinHeapNode root = minHeap->array[0];
    MinHeapNode lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root.vertex] = minHeap->size - 1;
    minHeap->pos[lastNode.vertex] = 0;

    --(minHeap->size);
    minHeapify(minHeap, 0);

    return(root);
}

void decreaseKey(MinHeap* minHeap, int vt, int dist, int step)
{
    int i = minHeap->pos[vt];
    minHeap->array[i].distance = dist;
    minHeap->array[i].step = step;

    while (i && minHeap->array[i].distance < minHeap->array[(i - 1) / 2].distance)
    {
        minHeap->pos[minHeap->array[i].vertex] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2].vertex] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

void printPath(int parent[][10], int destination, int step)
{
    if (parent[destination][step] == -1)
    {
        return;
    }
    printPath(parent, parent[destination][step], (step - 1 + N) % N);
    printf("%d ", destination);
}

void dijkstra(int source, int destination)
{
    int dist[MAX_VERTICES][10]; // Distance array considering steps
    int parent[MAX_VERTICES][10]; // Parent array considering steps
    MinHeap* minHeap = createMinHeap(MAX_VERTICES * 10); // Increased capacity

    for (int vt = 0; vt < V; vt++)
    {
        for (int step = 0; step < N; step++)
        {
            dist[vt][step] = INF;
            minHeap->array[vt * N + step].vertex = vt;
            minHeap->array[vt * N + step].distance = INF;
            minHeap->array[vt * N + step].step = step;
            minHeap->pos[vt * N + step] = vt * N + step;
            parent[vt][step] = -1;
        }
    }

    minHeap->array[source * N].distance = 0;
    dist[source][0] = 0;
    decreaseKey(minHeap, source * N, 0, 0);
    minHeap->size = V * N;

    while (!isEmpty(minHeap))
    {
        MinHeapNode minHeapNode = extractMin(minHeap);
        int vs = minHeapNode.vertex;
        int currentStep = minHeapNode.step;

        for (int i = 0; i < graph[vs].edge_count; i++)
        {
            Edge edge = graph[vs].edges[i];
            int vt = edge.target;
            int nextStep = (currentStep + 1) % N;
            int weight = edge.weights[currentStep];

            if (dist[vs][currentStep] != INF && dist[vs][currentStep] + weight < dist[vt][nextStep])
            {
                dist[vt][nextStep] = dist[vs][currentStep] + weight;
                parent[vt][nextStep] = vs;
                decreaseKey(minHeap, vt * N + nextStep, dist[vt][nextStep], nextStep);
            }
        }
    }

    // Find the minimum distance to the destination considering all steps
    int min_dist = INF;
    int min_step = -1;
    for (int step = 0; step < N; step++)
    {
        if (dist[destination][step] < min_dist)
        {
            min_dist = dist[destination][step];
            min_step = step;
        }
    }

    printf("%d ", source);
    printPath(parent, destination, min_step);

    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);
}

void readGraph(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &V, &N); // First line of graph.txt

    for (int i = 0; i < V; i++)
    {
        graph[i].edge_count = 0;
    }

    int vs, vt;
    while (fscanf(file, "%d %d", &vs, &vt) != EOF) // Source vertex and target vertex (next lines of graph.txt)
    {
        Edge edge;
        edge.target = vt;
        for (int i = 0; i < N; i++)
        {
            fscanf(file, "%d", &edge.weights[i]); // Weight(s)
        }
        graph[vs].edges[graph[vs].edge_count++] = edge;
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <points file>\n", argv[0]);
        return(1);
    }

    readGraph(argv[1]);

    int source, destination;
    while (scanf("%d %d", &source, &destination) == 2) // User input
    {
        dijkstra(source, destination);
    }

    return(0);
}