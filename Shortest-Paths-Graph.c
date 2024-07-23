#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 1000
#define INF INT_MAX

typedef struct {
    int target;
    int weights[10];
} Edge;

typedef struct {
    Edge edges[MAX_VERTICES];
    int edge_count;
} Vertex;

Vertex graph[MAX_VERTICES];
int V, N;

typedef struct {
    int vertex;
    int distance;
    int step;
} MinHeapNode;

typedef struct {
    MinHeapNode *array;
    int size;
    int capacity;
    int *pos;
} MinHeap;

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode*)malloc(capacity * sizeof(MinHeapNode));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode* a, MinHeapNode* b) {
    MinHeapNode t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left].distance < minHeap->array[smallest].distance)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right].distance < minHeap->array[smallest].distance)
        smallest = right;

    if (smallest != idx) {
        MinHeapNode smallestNode = minHeap->array[smallest];
        MinHeapNode idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode.vertex] = idx;
        minHeap->pos[idxNode.vertex] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

MinHeapNode extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return (MinHeapNode){-1, INF, -1};

    MinHeapNode root = minHeap->array[0];
    MinHeapNode lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root.vertex] = minHeap->size - 1;
    minHeap->pos[lastNode.vertex] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(MinHeap* minHeap, int v, int dist, int step) {
    int i = minHeap->pos[v];
    minHeap->array[i].distance = dist;
    minHeap->array[i].step = step;

    while (i && minHeap->array[i].distance < minHeap->array[(i - 1) / 2].distance) {
        minHeap->pos[minHeap->array[i].vertex] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2].vertex] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap *minHeap, int v) {
    return minHeap->pos[v] < minHeap->size;
}

void printPath(int parent[][10], int j, int step) {
    if (parent[j][step] == -1)
        return;
    printPath(parent, parent[j][step], (step - 1 + N) % N);
    printf("%d ", j);
}

void dijkstra(int src, int dest) {
    int dist[MAX_VERTICES][10];  // distance array considering steps
    int parent[MAX_VERTICES][10];  // parent array considering steps
    MinHeap* minHeap = createMinHeap(MAX_VERTICES * 10);  // increased capacity

    for (int v = 0; v < V; ++v) {
        for (int step = 0; step < N; ++step) {
            dist[v][step] = INF;
            minHeap->array[v * N + step].vertex = v;
            minHeap->array[v * N + step].distance = INF;
            minHeap->array[v * N + step].step = step;
            minHeap->pos[v * N + step] = v * N + step;
            parent[v][step] = -1;
        }
    }

    minHeap->array[src * N].distance = 0;
    dist[src][0] = 0;
    decreaseKey(minHeap, src * N, 0, 0);
    minHeap->size = V * N;

    while (!isEmpty(minHeap)) {
        MinHeapNode minHeapNode = extractMin(minHeap);
        int u = minHeapNode.vertex;
        int currentStep = minHeapNode.step;

        for (int i = 0; i < graph[u].edge_count; ++i) {
            Edge edge = graph[u].edges[i];
            int v = edge.target;
            int nextStep = (currentStep + 1) % N;
            int weight = edge.weights[currentStep];

            if (dist[u][currentStep] != INF && dist[u][currentStep] + weight < dist[v][nextStep]) {
                dist[v][nextStep] = dist[u][currentStep] + weight;
                parent[v][nextStep] = u;
                decreaseKey(minHeap, v * N + nextStep, dist[v][nextStep], nextStep);
            }
        }
    }

    // Find the minimum distance to the destination considering all steps
    int minDist = INF;
    int minStep = -1;
    for (int step = 0; step < N; ++step) {
        if (dist[dest][step] < minDist) {
            minDist = dist[dest][step];
            minStep = step;
        }
    }

    if (minDist == INF) {
        printf("No path from %d to %d\n", src, dest);
    } else {
        printf("Path: %d ", src);
        printPath(parent, dest, minStep);
        printf("\nTotal weight: %d\n", minDist);
    }

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

    fscanf(file, "%d %d", &V, &N);

    for (int i = 0; i < V; ++i)
    {
        graph[i].edge_count = 0;
    }

    int u, v;
    while (fscanf(file, "%d %d", &u, &v) != EOF)
    {
        Edge edge;
        edge.target = v;
        for (int i = 0; i < N; i++)
        {
            fscanf(file, "%d", &edge.weights[i]);
        }
        graph[u].edges[graph[u].edge_count++] = edge;
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return(1);
    }

    readGraph(argv[1]);

    int src, dest;
    while (scanf("%d %d", &src, &dest) == 2)
    {
        dijkstra(src, dest);
    }

    return(0);
}