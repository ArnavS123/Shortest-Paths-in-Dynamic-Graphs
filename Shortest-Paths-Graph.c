#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 2000
#define MAX_EDGES MAX_VERTICES
#define BIG 99999

struct Edge
{
    int target;
    int weights[10];
};

struct Vertex
{
    struct Edge edges[MAX_EDGES];
    int num_edges;
};

struct Node
{
    int vertex;
    int distance;
    int step;
};

struct Heap
{
    struct Node *arr;
    int curr_size;
    int max_size;
    int *pos;
};

struct Vertex graph[MAX_VERTICES];
int V, N; // V is the number of vertices in the graph and N is the period of the edge weights

struct Heap* build_heap(int max_size)
{
    struct Heap* heap = (struct heap*)malloc(sizeof(struct heap*));
    heap->pos = (int *)malloc(max_size * sizeof(int));
    heap->curr_size = 0;
    heap->max_size = max_size;
    heap->arr = (struct Node*)malloc(max_size * sizeof(struct Node));

    return(heap);
}

void swap(struct Node* a, struct Node* b)
{
    struct Node temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(struct Heap* heap, int ind)
{
    int left = ind * 2 + 1;
    int right = ind * 2 + 2;
    int min = ind;
 
    if (left >= heap->curr_size || left < 0)
    {
        left = -1;
    }
    if (right >= heap->curr_size || right < 0)
    {
        right = -1;
    }

    if (left != -1 && heap->arr[left].distance < heap->arr[ind].distance)
    {
        min = left;
    }
    if (right != -1 && heap->arr[right].distance < heap->arr[min].distance)
    {
        min = right;
    }

    if (min != ind)
    {
        heap->pos[heap->arr[min].vertex] = ind;
        heap->pos[heap->arr[ind].vertex] = min;

        swap(&heap->arr[min], &heap->arr[ind]);

        heapify(heap, min);
    }
}

struct Node extract_min(struct Heap* heap)
{
    if (heap->curr_size == 0)
    {
        struct Node empty = {-999, BIG, -999};
        return(empty);
    }

    struct Node root = heap->arr[0];
    struct Node last_node = heap->arr[heap->curr_size - 1];
    heap->arr[0] = last_node;

    heap->pos[root.vertex] = heap->curr_size - 1;
    heap->pos[last_node.vertex] = 0;

    (heap->curr_size)--;
    heapify(heap, 0);

    return(root);
}

void decrease_key(struct Heap* heap, int vert_target, int dist, int step)
{
    int index = heap->pos[vert_target];
    heap->arr[index].distance = dist;
    heap->arr[index].step = step;

    while (index > 0)
    {
        int parent_index = (index - 1) / 2; // Index of parent node

        if (heap->arr[index].distance >= heap->arr[parent_index].distance)
        {
            break;
        }

        heap->pos[heap->arr[index].vertex] = parent_index;
        heap->pos[heap->arr[parent_index].vertex] = index;

        swap(&heap->arr[index], &heap->arr[parent_index]);

        index = parent_index;
    }
}

void print_path(int parent[][10], int destination, int step)
{
    if (parent[destination][step] == -1)
    {
        return;
    }
    
    print_path(parent, parent[destination][step], (step - 1 + N) % N);
    printf("%d ", destination);
}

void dijkstra(int source, int destination)
{
    int dist[MAX_VERTICES][10];
    int parent[MAX_VERTICES][10];
    struct Heap* heap = build_heap(MAX_VERTICES);

    for (int vert_target = 0; vert_target < V; vert_target++)
    {
        for (int step = 0; step < N; step++)
        {
            int ind = vert_target * N + step;
            dist[vert_target][step] = BIG;
            heap->arr[ind].vertex = ind;
            heap->arr[ind].distance = BIG;
            heap->arr[ind].step = step;
            heap->pos[ind] = ind;
            parent[vert_target][step] = -1;
        }
    }

    heap->arr[source * N].distance = 0;
    dist[source][0] = 0;
    decrease_key(heap, source * N, 0, 0);
    heap->curr_size = V * N;

    while (heap->curr_size != 0)
    {
        struct Node node = extract_min(heap);
        int vert_source = node.vertex / N;
        int currentStep = node.step;

        for (int i = 0; i < graph[vert_source].num_edges; i++)
        {
            struct Edge edge = graph[vert_source].edges[i];
            int vert_target = edge.target;
            int next_step = (currentStep + 1) % N;
            int weight = edge.weights[currentStep];

            if (dist[vert_source][currentStep] != BIG && dist[vert_source][currentStep] + weight < dist[vert_target][next_step])
            {
                dist[vert_target][next_step] = dist[vert_source][currentStep] + weight;
                parent[vert_target][next_step] = vert_source;
                decrease_key(heap, vert_target * N + next_step, dist[vert_target][next_step], next_step);
            }
        }
    }

    // Find the minimum distance to the destination considering all steps
    int min_dist = BIG;
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
    print_path(parent, destination, min_step);
    printf("\n");

    free(heap->pos);
    free(heap->arr);
    free(heap);
}

void read_graph(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &V, &N);

    for (int i = 0; i < V; i++)
    {
        graph[i].num_edges = 0;
    }

    int vert_source;
    int vert_target;
    while (fscanf(file, "%d %d", &vert_source, &vert_target) != EOF)
    {
        struct Edge edge;
        edge.target = vert_target;
        for (int i = 0; i < N; i++)
        {
            fscanf(file, "%d", &edge.weights[i]);
        }
        graph[vert_source].edges[graph[vert_source].num_edges++] = edge;
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error with files!\n");
        return(1);
    }

    read_graph(argv[1]);

    int source;
    int destination;

    while (scanf("%d %d", &source, &destination) == 2)
    {
        dijkstra(source, destination);
    }

    return(0);
}