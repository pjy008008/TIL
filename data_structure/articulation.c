#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node
{
    int vertex;
    struct Node *next;
} Node;

Node *createNode(int v)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

void DFS(Node **adjList, int v, int *visited, int *disc, int *low, int *parent, int *ap)
{
   
    static int time = 0;
    int children = 0;

    visited[v] = 1;  
    disc[v] = low[v] = ++time;

    Node *currentNode = adjList[v];
    while (currentNode != NULL)
    {
        int neighbor = currentNode->vertex;

        if (!visited[neighbor])
        {
            children++;
            parent[neighbor] = v;
            DFS(adjList, neighbor, visited, disc, low, parent, ap);
            if (parent[v] == -1 && children > 1)
                ap[v] = 1;
            if (parent[v] != -1 && low[neighbor] >= disc[v])
                ap[v] = 1;
            low[v] = (low[v] < low[neighbor]) ? low[v] : low[neighbor];
        }
        else if (neighbor != parent[v])
        {
            low[v] = (low[v] < disc[neighbor]) ? low[v] : disc[neighbor];
        }

        currentNode = currentNode->next;
    }
}

void findArticulationPoints(Node **adjList, int vertices)
{
    int visited[200];
    int disc[200];
    int low[200];
    int parent[200];
    int ap[200];
    for (int i = 0; i < vertices; i++)
    {
        visited[i] = 0;
        disc[i] = 0;
        low[i] = 0;
        parent[i] = -1;
        ap[i] = 0;
    }

    for (int i = 0; i < vertices; i++)
    {
        if (!visited[i])
            DFS(adjList, i, visited, disc, low, parent, ap);
    }

    printf("AP: ");
    for (int i = 0; i < vertices; i++)
    {
        if (ap[i])
            printf("%d ", i);
    }
    printf("\n");
}

void getAP()
{
    int vertices = 200;
    Node **adjList = (Node **)malloc(vertices * sizeof(Node *));

    char str1[2000];
    FILE *pFile = fopen("AP.txt", "r");
    fgets(str1, 2000, pFile);
    fclose(pFile);

    char *tk = strtok(str1, "$(),");
    while (tk != NULL)
    {
        int src = atoi(tk);
        tk = strtok(NULL, "$(),");
        if (tk == NULL)
        {
            break;
        }
        int dest = atoi(tk);
        tk = strtok(NULL, "$(),");

        Node *newNode = createNode(dest);
        newNode->next = adjList[src];
        adjList[src] = newNode;

        newNode = createNode(src);
        newNode->next = adjList[dest];
        adjList[dest] = newNode;
    }

    findArticulationPoints(adjList, vertices);
}

int main()
{
    clock_t start = clock();
    getAP();
    clock_t end = clock();
    printf("소요 시간: %lf\n", (double)(end - start) / (clock_t)1000);
    return 0;
}