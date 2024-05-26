#include <stdio.h>
#include <stdlib.h> // For malloc, realloc, free functions.
#include <stdbool.h> // For bool data type.
#include <string.h> // For memcpy, memcmp, qsort functions.

#define MAX_NODES 100 // Maximum number of nodes in the graph.

typedef struct // Edge structure.
{
    int node;   // Node index.
    int weight; // Edge weight.
} Edge;

typedef struct // Graph structure.
{
    Edge *edges[MAX_NODES];   // Array of edges.
    int edgeCount[MAX_NODES]; // Number of edges for each node.
} Graph;

typedef struct // Polygon structure.
{
    int *nodes;    // Array of nodes.
    int nodeCount; // Number of nodes.
    int perimeter; // Perimeter of the polygon.
} Polygon;

/*
@brief creates a graph structure with N nodes.

@param N number of nodes in the graph.

@return a pointer to the created graph.
*/
Graph *createGraph(int N) // Create a graph with N nodes.
{
    int i;
    Graph *graph = (Graph *)malloc(sizeof(Graph)); // Allocate memory for the graph.
    for (i = 0; i < N; i++) // Initialize the graph.
    {
        graph->edges[i] = NULL; // Set the edges to NULL.
        graph->edgeCount[i] = 0; // Set the edge count to 0.
    }
    return graph;
}

/*
@brief adds an edge between two nodes with a weight.

@param graph pointer to the graph.
@param u first node.
@param v second node.
@param weight weight of the edge.
*/
void addEdge(Graph *graph, int u, int v, int weight) // Add an edge between two nodes.
{
    Edge *edgeUV = (Edge *)malloc(sizeof(Edge)); // Allocate memory for the edge.
    edgeUV->node = v; // Set the node.
    edgeUV->weight = weight; // Set the weight.
    graph->edges[u] = (Edge *)realloc(graph->edges[u], (graph->edgeCount[u] + 1) * sizeof(Edge)); // Reallocate memory for the edges.
    graph->edges[u][graph->edgeCount[u]++] = *edgeUV; // Add the edge to the graph.

    Edge *edgeVU = (Edge *)malloc(sizeof(Edge)); // Allocate memory for the edge.
    edgeVU->node = u; // Set the node.
    edgeVU->weight = weight; // Set the weight.
    graph->edges[v] = (Edge *)realloc(graph->edges[v], (graph->edgeCount[v] + 1) * sizeof(Edge)); // Reallocate memory for the edges.
    graph->edges[v][graph->edgeCount[v]++] = *edgeVU; // Add the edge to the graph.
}

/*
@brief performs a depth-first search on the graph to find all polygons.

@param graph pointer to the graph.
@param start start node.
@param node current node.
@param parent parent node.
@param visited array of visited nodes.
@param stack array of nodes in the current path.
@param stackIndex index of the stack.
@param polygons array of polygons.
@param polygonCount number of polygons.
*/
void dfs(Graph *graph, int start, int node, int parent, bool *visited, int *stack, int *stackIndex, Polygon *polygons, int *polygonCount) // Depth-first search.
{
	int i, j, k;
    visited[node] = true; // Mark the node as visited.
    stack[(*stackIndex)++] = node; // Add the node to the stack.

    for (i = 0; i < graph->edgeCount[node]; i++) // Traverse the edges.
    {
        int neighbour = graph->edges[node][i].node; // Get the neighbour node.
        if (!visited[neighbour]) // Check if the neighbour is not visited.
        {
            dfs(graph, start, neighbour, node, visited, stack, stackIndex, polygons, polygonCount); // Recursive call.
        }
        else if (neighbour == start && *stackIndex >= 3) // Check if the neighbour is the start node and the stack has at least 3 nodes.
        {
            Polygon polygon; // Create a polygon.
            polygon.nodes = (int *)malloc(*stackIndex * sizeof(int)); // Allocate memory for the nodes.
            polygon.nodeCount = 0; // Set the node count to 0.
            polygon.perimeter = 0; // Set the perimeter to 0.

            // Copy nodes in the stack to the polygon
            for (j = 0; j < *stackIndex; j++) // Traverse the stack.
            {
                polygon.nodes[polygon.nodeCount++] = stack[j]; // Add the node to the polygon.
            }

        
            for (j = 0; j < polygon.nodeCount; j++) // Traverse the nodes.
            {
                bool exit = false; // Exit flag.
                k = 0; // Set the index to 0.
                while (!exit && k < graph->edgeCount[polygon.nodes[j]]) // Traverse the edges.
                {
                    if (graph->edges[polygon.nodes[j]][k].node == polygon.nodes[(j + 1) % polygon.nodeCount]) // Check if the edge is found.
                    {
                        polygon.perimeter += graph->edges[polygon.nodes[j]][k].weight; // Add the weight to the perimeter.
                        exit = true; // Set the exit flag.
                    }
                    k++; // Increase the index.
                }
            }
            polygons[(*polygonCount)++] = polygon; // Add the polygon to the array of polygons.
        }
    }

    visited[node] = false; // Mark the node as not visited.
    (*stackIndex)--; // Decrease the stack index.
}

/*
@brief finds all the shapes (polygons) in the graph.

@param graph pointer to the graph.
@param nodeCount number of nodes in the graph.
@param polygons array of polygons.
@param polygonCount number of polygons.
*/
void findShapes(Graph *graph, int nodeCount, Polygon *polygons, int *polygonCount) // Find all shapes in the graph.
{
	int i;
    bool visited[MAX_NODES] = {false}; // Array of visited nodes.
    int stack[MAX_NODES]; // Array of nodes in the current path.
    int stackIndex = 0; // Index of the stack.

    for (i = 0; i < nodeCount; i++) // Traverse the nodes.
    {
        dfs(graph, i, i, -1, visited, stack, &stackIndex, polygons, polygonCount); // Depth-first search.
    }
}

/*
@brief checks if two polygons are the same.

@param poly1 first polygon.
@param poly2 second polygon.

@return true if the polygons are the same, false otherwise.
*/
bool arePolygonsSame(Polygon *poly1, Polygon *poly2) // Check if two polygons are the same.
{
    if (poly1->nodeCount != poly2->nodeCount) // Check if the number of nodes are different.
        return false;

    int *nodes1 = (int *)malloc(poly1->nodeCount * sizeof(int)); // Allocate memory for the nodes.
    int *nodes2 = (int *)malloc(poly2->nodeCount * sizeof(int)); 

    memcpy(nodes1, poly1->nodes, poly1->nodeCount * sizeof(int)); // Copy the nodes.
    memcpy(nodes2, poly2->nodes, poly2->nodeCount * sizeof(int));

    qsort(nodes1, poly1->nodeCount, sizeof(int), (int (*)(const void *, const void *))strcmp); // Sort the nodes.
    qsort(nodes2, poly2->nodeCount, sizeof(int), (int (*)(const void *, const void *))strcmp); 

    bool same = memcmp(nodes1, nodes2, poly1->nodeCount * sizeof(int)) == 0; // Check if the nodes are the same.

    free(nodes1); // Free the memory.
    free(nodes2);

    return same; // Return the result.
}

/*
@brief removes duplicate polygons from the list of polygons.

@param polygons array of polygons.
@param polygonCount pointer to the number of polygons.
*/
void removeDuplicates(Polygon *polygons, int *polygonCount) // Remove duplicate polygons.
{
	int i, j, k;
    for (i = 0; i < *polygonCount; i++) // Traverse the polygons.
    {
        for (j = i + 1; j < *polygonCount; j++) // Traverse the polygons.
        {
            if (arePolygonsSame(&polygons[i], &polygons[j])) // Check if the polygons are the same.
            {
                for (k = j; k < *polygonCount - 1; k++) // Traverse the polygons.
                {
                    polygons[k] = polygons[k + 1]; // Shift the polygons.
                }
                (*polygonCount)--; // Decrease the polygon count.
                j--; // Decrease the index.
            }
        }
    }
}

int main()
{
    int i, j;
    char u[3], v[3]; // Nodes.
    int weight; // Edge weight.
    int nodeMap[MAX_NODES] = {0}; // Map of nodes.
    int nodeCount = 0; // Number of nodes.
    int polygonCount; // Number of polygons.
    int triangleCount, quadrilateralCount, pentagonCount; // Number of triangles, quadrilaterals, pentagons.
    int triangleIndex, quadrilateralIndex, pentagonIndex; // Index of triangles, quadrilaterals, pentagons.
    char fileName[100]; // File name.

    printf("Enter the file name: ");
    scanf("%s", fileName); // Get the file name.

    FILE *file = fopen(fileName, "r"); // Open the file in read mode.
    if (file == NULL) // Check if the file cannot be opened.
    {
        printf("File cannot be opened.\n");
        return 1;
    }

    Graph *graph = createGraph(MAX_NODES); // Create a graph with MAX_NODES nodes.

    while (fscanf(file, "%s %s %d", u, v, &weight) != EOF) // Read the nodes and the weight from the file.
    {
        int uIndex = u[0] - 'A'; // Get the index of the first node.
        int vIndex = v[0] - 'A'; // Get the index of the second node.
        addEdge(graph, uIndex, vIndex, weight); // Add an edge between the nodes.

        if (!nodeMap[uIndex]) // Check if the node is already added.
        {
            nodeMap[uIndex] = 1; // Mark the node as added.
            nodeCount++;         // Increase the node count.
        }
        if (!nodeMap[vIndex]) // Check if the node is already added.
        {
            nodeMap[vIndex] = 1; // Mark the node as added.
            nodeCount++;         // Increase the node count.
        }
    }
    fclose(file); // Close the file.

    Polygon polygons[MAX_NODES]; // Array of polygons.
    polygonCount = 0; // Set the polygon count to 0.

    findShapes(graph, MAX_NODES, polygons, &polygonCount); // Find all shapes in the graph.

    removeDuplicates(polygons, &polygonCount); // Remove duplicate polygons.

    triangleCount = 0, quadrilateralCount = 0, pentagonCount = 0; // Set the number of triangles, quadrilaterals, pentagons to 0.
    for (i = 0; i < polygonCount; i++) // Traverse the polygons.
    { 
        if (polygons[i].nodeCount == 3) triangleCount++; // Check the number of nodes.
        else if (polygons[i].nodeCount == 4) quadrilateralCount++;
        else if (polygons[i].nodeCount == 5) pentagonCount++;
    }

    printf("Number of Polygons: %d\n", polygonCount); // Print the number of polygons.
    printf("Number of 3-gons: %d\n", triangleCount);
    printf("Number of 4-gons: %d\n", quadrilateralCount);
    printf("Number of 5-gons: %d\n", pentagonCount);

    triangleIndex = 1, quadrilateralIndex = 1, pentagonIndex = 1; // Set the index of triangles, quadrilaterals, pentagons to 1.
    for (i = 0; i < polygonCount; i++) // Traverse the polygons.
    {
        if (polygons[i].nodeCount == 3) // Check the number of nodes.
        {
            printf("%d. 3-gon: ", triangleIndex++);
        }
        else if (polygons[i].nodeCount == 4)
        {
            printf("%d. 4-gon: ", quadrilateralIndex++);
        }
        else if (polygons[i].nodeCount == 5)
        {
            printf("%d. 5-gon: ", pentagonIndex++);
        }
        for (j = 0; j < polygons[i].nodeCount; j++) 
        {
            printf(" %c", polygons[i].nodes[j] + 'A'); // Print the nodes.
        }
        printf(" %c Length: %d\n", polygons[i].nodes[0] + 'A', polygons[i].perimeter); // Print the length.
    }

    // Free the memory.
    for (i = 0; i < nodeCount; i++) 
    {
        free(graph->edges[i]);
    }
    free(graph);

    // Free the memory.
    for (i = 0; i < polygonCount; i++)
    {
        free(polygons[i].nodes);
    }

    return 0;
}
