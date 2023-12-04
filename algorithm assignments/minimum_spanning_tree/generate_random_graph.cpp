#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

// 간선을 나타내는 구조체
struct Edge {
    int source, destination, weight;
};

// 그래프를 생성하는 함수
vector<Edge> generateRandomGraph(int numVertices, int numEdges) {
    // 난수 생성기 초기화
    mt19937 rng(time(0));
    uniform_int_distribution<int> vertexDist(0, numVertices - 1);
    uniform_int_distribution<int> weightDist(1, 20); // 가중치는 1에서 20까지의 무작위 값으로 설정

    vector<Edge> edges;

    for (int i = 0; i < numEdges; ++i) {
        Edge edge;
        edge.source = vertexDist(rng);
        edge.destination = vertexDist(rng);
        edge.weight = weightDist(rng);

        edges.push_back(edge);
    }

    return edges;
}

int main() {
    int numVertices, numEdges;

    cout << "Enter the number of vertices: ";
    cin >> numVertices;

    cout << "Enter the number of edges: ";
    cin >> numEdges;

    // 무작위 그래프 생성
    vector<Edge> randomEdges = generateRandomGraph(numVertices, numEdges);

    // 생성된 그래프 출력
    cout << "Generated Random Graph:\n";
    for (const Edge& edge : randomEdges) {
        cout << edge.source << " " << edge.destination << " " << edge.weight << "\n";
    }

    return 0;
}