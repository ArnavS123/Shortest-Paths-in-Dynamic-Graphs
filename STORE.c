#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX
#define MAX_LINES 20000
#define MAX_WEIGHTS 10

typedef struct
{
    int vs; // vertex source (starting vertex)
    int vt; // vertex target (ending vertex)
    int weights[MAX_WEIGHTS]; // Edge weights
} Edge;

typedef struct
{
    int V; // Number of vertices in the graph
    int N; // Number of edge weights
    Edge edges[MAX_LINES]; // Array of edges
} Data;

typedef struct
{
    int vertex; // Vertex number
    int distance; // Current shortest distance from vs to vt
    int step; // Step counter
} Node;

typedef struct
{
    Node *arr; // Array of nodes
    int curr_size; // Current number of minheap elements
    int capacity; // Maximum minheap capacity (max_size)
} Heap;

Heap* build_heap(int max_size)
{
    // Initalize and allocate memory
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->curr_size = 0;
    heap->capacity = max_size;
    heap->arr = (Node*)malloc(max_size * sizeof(Node));

    if (heap == NULL || heap->arr == NULL)
    {
        printf("Memory error!\n");
        return(NULL);
    }

    return(heap);
}

void heapify(Heap* heap, int ind)
{
    int left_child = (ind * 2) + 1;
    int right_child = (ind * 2) + 2;
    int min = ind;

    if (left_child < heap->curr_size) // Is left_child in range
    {
        if (heap->arr[left_child].distance < heap->arr[min].distance) // Is left_child smaller than ind
        {
            min = left_child; // left_child is minimum index
        }
    }

    if (right_child < heap->curr_size) // Is right_child in range
    {
        if (heap->arr[right_child].distance < heap->arr[min].distance) // Is right_child smaller than min
        {
            min = right_child; // right_child is minimum index
        }
    }

    if (min != ind)
    {
        // Swap heap->arr[min] and heap->arr[ind]
        Node temp = heap->arr[min];
        heap->arr[min] = heap->arr[ind];
        heap->arr[ind] = temp;
        
        heapify(heap, min); // rerun function with min instead of ind
    }
}

Node extract_min(Heap* heap)
{
    if (heap->curr_size == 0)
    {
        printf("Minheap empty\n");
        Node null = {-INF, INF, -INF};
        return(null);
    }

    Node extract_root = heap->arr[0]; // Extract min element (min at root of minheap)

    // Just in case heap is just the root
    if (heap->curr_size == 1)
    {
        (heap->curr_size)--;
        return(extract_root);
    }

    // Put last_node at root of minheap
    int end = (heap->curr_size) - 1;
    Node last_node = heap->arr[end];
    heap->arr[0] = last_node; // Replacement

    (heap->curr_size)--; // Decrease size of minheap (remove 2nd instance of last_node)
    heapify(heap, 0); // Rebalance minheap

    return(extract_root);
}

void decrease_key(Heap* heap, int vertex_to_update, int new_distance, int new_step)
{
    // Find index of vertex_to_update
    int ind = -1;
    for (int i = 0; i < heap->curr_size; i++)
    {
        if (heap->arr[i].vertex == vertex_to_update)
        {
            ind = i;
            break;
        }
    }

    // Check if vertex is in heap
    if (ind == -1 || ind >= heap->curr_size)
    {
        return;
    }

    // Update distance and step of vertex_to_update
    heap->arr[ind].distance = new_distance;
    heap->arr[ind].step = new_step;

    while (ind > 0)
    {
        int parent_ind = (ind - 1) / 2; // Parent index
        if (heap->arr[ind].distance >= heap->arr[parent_ind].distance) // Check if minheap property is violated
        {
            break;
        }
        else
        {
            // Swap heap->arr[ind] and heap->arr[ind / 2]
            Node temp = heap->arr[ind];
            heap->arr[ind] = heap->arr[parent_ind];
            heap->arr[parent_ind] = temp;

            ind = parent_ind;
        }
    }
}

void dijkstra(int source, int destination, Data data)
{
    int distances[data.V];
    int previous[data.V]; // Array to track the path
    int visited[data.V];

    for (int i = 0; i < data.V; i++) {
        distances[i] = INF;
        visited[i] = 0;
        previous[i] = -1; // Initialize previous array
    }

    distances[source] = 0;

    Heap* minheap = build_heap(MAX_LINES);

    for (int i = 0; i < data.V; i++) {
        Node node = {i, distances[i], 0};
        minheap->arr[i] = node;
        minheap->curr_size++;
    }

    while (minheap->curr_size > 0)
    {
        Node minNode = extract_min(minheap);
        int u = minNode.vertex;

        if (u == destination) {
            printf("Shortest path distance: %d\n", distances[destination]);

            // Output the path
            int path[MAX_LINES];
            int path_index = 0;
            for (int at = destination; at != -1; at = previous[at]) {
                path[path_index++] = at;
            }
            for (int i = path_index - 1; i >= 0; i--) {
                printf("%d ", path[i]);
            }
            printf("\n");
            break;
        }

        if (visited[u]) continue;
        visited[u] = 1;

        for (int i = 0; i < MAX_LINES; i++)
        {
            if (data.edges[i].vs == u)
            {
                int v = data.edges[i].vt;
                // Select weight based on current step using modulo operation
                int weight = data.edges[i].weights[minNode.step % data.N]; 

                if (!visited[v] && distances[u] != INF && distances[u] + weight < distances[v])
                {
                    distances[v] = distances[u] + weight;
                    previous[v] = u; // Track the path
                    decrease_key(minheap, v, distances[v], minNode.step + 1);
                }
            }
        }
    }

    free(minheap->arr);
    free(minheap);
}

Data read_data(const char *filename)
{
    FILE *data_file = fopen(filename, "r");
    if (data_file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Data data;
    fscanf(data_file, "%d %d", &data.V, &data.N); // Read V (num of vertices) and N (num of weights)

    int i = 0;
    while (fscanf(data_file, "%d %d", &data.edges[i].vs, &data.edges[i].vt) == 2) // Read vertex sources and targets
    {
        for (int j = 0; j < data.N; j++)
        {
            fscanf(data_file, "%d", &data.edges[i].weights[j]); // Read weights
        }
        i++;
    }

    fclose(data_file);
    return(data);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "ERROR!\n", argv[0]);
        return(EXIT_FAILURE);
    }

    Data data = read_data(argv[1]); // Read data_file

    int source;
    int dest;
    while (scanf("%d %d", &source, &dest) == 2) // User input
    {
        dijkstra(source, dest, data); // Dijkstra's algorithm
    }

    return(EXIT_SUCCESS);
}