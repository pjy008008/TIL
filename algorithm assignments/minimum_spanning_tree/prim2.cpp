#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

// 그래프의 간선을 나타내는 구조체
struct Edge {
    int to;
    int weight;
};

// 프림 알고리즘을 이용한 최소 신장 트리 찾기
int primMST(const vector<vector<Edge>>& graph, int start) {
    int numVertices = graph.size();

    // 최소 힙을 이용하여 간선을 저장
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> minHeap;

    // 방문 여부를 나타내는 배열
    vector<bool> visited(numVertices, false);

    // 최소 신장 트리의 비용
    int totalWeight = 0;

    // 시작 정점을 방문하고 연결된 간선을 힙에 추가
    visited[start] = true;
    for (const Edge& edge : graph[start]) {
        minHeap.push({edge.weight, edge.to});
    }

    // 나머지 정점을 방문하면서 최소 신장 트리를 구성
    while (!minHeap.empty()) {
        pair<int, int> current = minHeap.top();
        minHeap.pop();

        int currentVertex = current.second;
        int currentWeight = current.first;

        // 이미 방문한 정점이면 무시
        if (visited[currentVertex]) {
            continue;
        }

        // 정점을 방문하고 해당 간선의 가중치를 더함
        visited[currentVertex] = true;
        totalWeight += currentWeight;

        // 현재 정점과 연결된 간선을 힙에 추가
        for (const Edge& edge : graph[currentVertex]) {
            if (!visited[edge.to]) {
                minHeap.push({edge.weight, edge.to});
            }
        }
    }

    return totalWeight;
}

int main() {
    // 그래프의 정점 수와 간선 수
    int numVertices = 20000;
    int numEdges = 100000;

    // 그래프를 나타내는 인접 리스트
    vector<vector<Edge>> graph(numVertices);

    // 간선을 무작위로 생성하여 그래프에 추가
    for (int i = 0; i < numEdges; ++i) {
        int from = rand() % numVertices;
        int to = rand() % numVertices;
        int weight = rand() % 1000 + 1; // 가중치는 1부터 1000까지의 무작위 값

        // 자기 자신으로의 간선은 생성하지 않음
        if (from != to) {
            graph[from].push_back({to, weight});
            graph[to].push_back({from, weight});
        }
    }

    // 임의의 시작 정점에서 최소 신장 트리 찾기
    int startVertex = rand() % numVertices;
    int minSpanningTreeWeight = primMST(graph, startVertex);

    cout << "최소 신장 트리의 총 가중치: " << minSpanningTreeWeight << endl;

    return 0;
}
