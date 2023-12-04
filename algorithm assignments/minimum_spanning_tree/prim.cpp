#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

// 그래프의 정점과 간선을 나타내는 클래스
class Graph {
public:
    int V, E; // 정점과 간선 개수
    vector<vector<pair<int, int>>> adjList; // 간선 정보를 저장하는 인접 리스트

    Graph(int V, int E) : V(V), E(E) {
        adjList.resize(V);
    }

    // 무방향 그래프에 간선 추가
    void addEdge(int u, int v, int weight) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }

    // Prim 알고리즘을 사용하여 최소 신장 트리 찾기
    void primMST() {
        vector<bool> visited(V, false);
        priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> pq;

        int src = 0; // 시작 정점을 0으로 선택 (임의로 선택 가능)

        pq.push({0, {src, -1}}); // {가중치, {도착정점, 시작정점}}

        int mstWeight = 0; // 최소 신장 트리의 총 가중치

        cout << "최소 신장 트리 간선 정보:" << endl;

        while (!pq.empty()) {
            int u = pq.top().second.first;
            int parent = pq.top().second.second;
            int weight = pq.top().first;
            pq.pop();
            // 이미 방문한 정점은 무시
            if (visited[u]) {
                continue;
            }
            if (parent != -1) {
                // 최소 신장 트리 간선 출력
                cout << parent << " " << u << " " << weight << endl;
            }
            mstWeight += weight;
            visited[u] = true;

            for (const auto &neighbor : adjList[u]) {
                int v = neighbor.first;
                int w = neighbor.second;

                if (!visited[v]) {
                    pq.push({w, {v, u}});
                }
            }
        }
        // 최소 신장 트리의 가중치 출력
        cout << "최소 신장 트리의 가중치: " << mstWeight << endl;
    }
};

int main() {
    int V, E;
    cout << "정점의 개수를 입력하세요: ";
    cin >> V;
    cout << "간선의 개수를 입력하세요: ";
    cin >> E;

    srand(time(0)); // 랜덤 시드 초기화

    Graph graph(V, E);

    // 랜덤하게 간선 추가
    for (int i = 0; i < E; ++i) {
        int u = rand() % V; // 0부터 V-1까지의 랜덤한 정점 선택
        int v = rand() % V;
        int weight = rand() % 20 + 1; // 간선의 가중치는 1부터 20까지의 랜덤한 값

        graph.addEdge(u, v, weight);
    }

    // Prim 알고리즘을 사용하여 최소 신장 트리 찾기
    double start = clock();
    graph.primMST();
    double finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << duration << "초" << endl;
    

    return 0;
}
