#include <iostream>
#include <vector>
using namespace std;
//1932

int main() {
	int n;
	int x;
	vector<int> v1, v2;
	cin >> n;

	for (int i = 0; i < n; i++) {
		for (int j = n - i-1; j < n; j++) {
			cin >> x;
			v1.push_back(x);
		}
		if (i > 1) {

		}
	}
	return 0;
}