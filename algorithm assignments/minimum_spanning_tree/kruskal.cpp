#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>

using namespace std;

// 간선을 나타내는 구조체
struct Edge
{
    int source, destination, weight;
};

// 그래프를 생성하는 함수
vector<Edge> generateRandomGraph(int numVertices, int numEdges)
{
    // 난수 생성기 초기화
    mt19937 rng(time(0));
    uniform_int_distribution<int> vertexDist(0, numVertices - 1);
    uniform_int_distribution<int> weightDist(1, 20); // 가중치는 1에서 20까지의 무작위 값으로 설정

    vector<Edge> edges;

    for (int i = 0; i < numEdges; ++i)
    {
        Edge edge;
        edge.source = vertexDist(rng);
        edge.destination = vertexDist(rng);
        edge.weight = weightDist(rng);

        edges.push_back(edge);
    }

    return edges;
}

// 부모 노드를 찾는 함수 (Union-Find에서 사용)
int findParent(vector<int> &parent, int node)
{
    if (parent[node] == -1)
        return node;
    return findParent(parent, parent[node]);
}

// 두 개의 부분집합을 합치는 함수 (Union-Find에서 사용)
void unionSets(vector<int> &parent, int x, int y)
{
    int xSet = findParent(parent, x);
    int ySet = findParent(parent, y);
    parent[xSet] = ySet;
}

// 간선 비교 함수 (정렬을 위해 사용)
bool compareEdges(const Edge &edge1, const Edge &edge2)
{
    return edge1.weight < edge2.weight;
}

// Kruskal's 알고리즘 구현
void kruskalMST(vector<Edge> &edges, int numVertices)
{
    // 간선을 가중치로 오름차순 정렬
    sort(edges.begin(), edges.end(), compareEdges);

    // 각 노드의 부모를 저장하는 배열 (-1로 초기화)
    vector<int> parent(numVertices, -1);

    // 최소 신장 트리를 저장하는 배열
    vector<Edge> mst;

    for (const Edge &edge : edges)
    {
        int sourceSet = findParent(parent, edge.source);
        int destSet = findParent(parent, edge.destination);

        // 싸이클을 형성하지 않으면 간선을 선택하고 부모를 합침
        if (sourceSet != destSet)
        {
            mst.push_back(edge);
            unionSets(parent, sourceSet, destSet);
        }
    }

    // 최소 신장 트리 출력
    cout << "Minimum Spanning Tree:\n";
    for (const Edge &edge : mst)
    {
        cout << edge.source << " " << edge.destination << " " << edge.weight << "\n";
    }
}

int main()
{
    int numVertices, numEdges;

    cout << "Enter the number of vertices: ";
    cin >> numVertices;

    cout << "Enter the number of edges: ";
    cin >> numEdges;

    // 무작위 그래프 생성
    vector<Edge> randomEdges = generateRandomGraph(numVertices, numEdges);

    // Kruskal's 알고리즘 호출
    double start = clock();
    kruskalMST(randomEdges, numEdges);
    double finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << duration << "초" << endl;

    return 0;
}
