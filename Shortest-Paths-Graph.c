#include <stdio.h>
#include <stdlib.h>

void read_graph(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error openng file!\n");
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
        int edge_index = graph[vert_source].num_edges;
        graph[vert_source].num_edges++;
        graph[vert_source].edges[edge_index] = edge;
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