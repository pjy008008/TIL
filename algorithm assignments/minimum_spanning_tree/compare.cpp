#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <functional>
#include <queue>
#include <numeric>
#include <ctime>

using namespace std;
using namespace chrono;

// 간선을 나타내는 구조체
struct Edge {
    int source, destination, weight;

    // 비교 연산자 정의
    bool operator>(const Edge& other) const {
        return weight > other.weight;
    }
};


// 그래프를 나타내는 인접 리스트
using Graph = vector<vector<Edge>>;

// Disjoint Set을 표현하는 구조체
struct DisjointSet {
    vector<int> parent, rank;

    DisjointSet(int size) {
        parent.resize(size);
        iota(parent.begin(), parent.end(), 0);
        rank.assign(size, 0);
    }

    int find(int v) {
        if (v == parent[v])
            return v;
        return parent[v] = find(parent[v]);
    }

    void unionSets(int a, int b) {
        a = find(a);
        b = find(b);

        if (a != b) {
            if (rank[a] < rank[b])
                swap(a, b);
            parent[b] = a;
            if (rank[a] == rank[b])
                rank[a]++;
        }
    }
};

// 크루스칼 알고리즘
vector<Edge> kruskalMST(const Graph& graph) {
    int numVertices = graph.size();

    // 간선을 가중치에 따라 정렬
    vector<Edge> edges;
    for (const auto& neighbors : graph) {
        edges.insert(edges.end(), neighbors.begin(), neighbors.end());
    }
    sort(edges.begin(), edges.end());

    // 최소 신장 트리를 저장하는 배열
    vector<Edge> mst;

    // Disjoint Set 초기화
    DisjointSet disjointSet(numVertices);

    for (const Edge& edge : edges) {
        int sourceRoot = disjointSet.find(edge.source);
        int destRoot = disjointSet.find(edge.destination);

        // 싸이클을 방지하기 위해 연결되는 두 노드가 서로 다른 그룹에 속해 있을 때에만 간선을 추가
        if (sourceRoot != destRoot) {
            mst.push_back(edge);
            disjointSet.unionSets(sourceRoot, destRoot);
        }
    }

    return mst;
}

// 프림 알고리즘
vector<Edge> primMST(const Graph& graph) {
    int numVertices = graph.size();

    // 간선의 최소 가중치를 유지하는 우선순위 큐
    priority_queue<Edge, vector<Edge>, greater<Edge>> pq;

    // 방문 여부를 나타내는 배열
    vector<bool> visited(numVertices, false);

    // 임의의 시작 정점 선택
    int startVertex = 0;

    // 시작 정점을 큐에 추가
    visited[startVertex] = true;
    for (const Edge& edge : graph[startVertex]) {
        pq.push(edge);
    }

    // 최소 신장 트리를 저장하는 배열
    vector<Edge> mst;

    while (!pq.empty()) {
        Edge currentEdge = pq.top();
        pq.pop();

        int dest = currentEdge.destination;

        // 싸이클을 방지하기 위해 이미 방문한 노드는 무시
        if (visited[dest]) {
            continue;
        }

        // 최소 신장 트리에 간선 추가
        mst.push_back(currentEdge);
        visited[dest] = true;

        // 현재 노드에 연결된 모든 간선을 큐에 추가
        for (const Edge& edge : graph[dest]) {
            if (!visited[edge.destination]) {
                pq.push(edge);
            }
        }
    }

    return mst;
}

// 랜덤한 그래프 생성
Graph generateRandomGraph(int numVertices, int numEdges) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> vertexDist(0, numVertices - 1);
    uniform_int_distribution<int> weightDist(1, 20);

    Graph graph(numVertices);

    for (int i = 0; i < numEdges; ++i) {
        int source = vertexDist(gen);
        int destination = vertexDist(gen);

        // 자기 자신에게 가는 간선 방지
        while (source == destination) {
            destination = vertexDist(gen);
        }

        int weight = weightDist(gen);

        graph[source].push_back({source, destination, weight});
        graph[destination].push_back({destination, source, weight});
    }

    return graph;
}

int main() {
    int numVertices, numEdges;

    cout << "Enter the number of vertices: ";
    cin >> numVertices;

    cout << "Enter the number of edges: ";
    cin >> numEdges;

    // 무작위 그래프 생성
    Graph randomGraph = generateRandomGraph(numVertices, numEdges);

    // Kruskal 알고리즘 수행 시간 측정
    auto kruskalStart = high_resolution_clock::now();
    vector<Edge> kruskalMSTResult = kruskalMST(randomGraph);
    auto kruskalStop = high_resolution_clock::now();
    auto kruskalDuration = duration_cast<microseconds>(kruskalStop - kruskalStart);

    // Prim 알고리즘 수행 시간 측정
    auto primStart = high_resolution_clock::now();
    vector<Edge> primMSTResult = primMST(randomGraph);
    auto primStop = high_resolution_clock::now();
    auto primDuration = duration_cast<microseconds>(primStop - primStart);

    // 결과 출력
    cout << "Kruskal Minimum Spanning Tree:\n";
    for (const Edge& edge : kruskalMSTResult) {
        cout << edge.source << " -- " << edge.destination << " : " << edge.weight << "\n";
    }
    cout << "Kruskal Algorithm Execution Time: " << kruskalDuration.count() << " microseconds\n\n";

    cout << "Prim Minimum Spanning Tree:\n";
    for (const Edge& edge : primMSTResult) {
        cout << edge.source << " -- " << edge.destination << " : " << edge.weight << "\n";
    }
    cout << "Prim Algorithm Execution Time: " << primDuration.count() << " microseconds\n";

    return 0;
}
