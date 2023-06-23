#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
	int x;
	vector<int> v;
	for (int i = 0; i < 28; i++) {
		cin >> x;
		v.push_back(x);
	}
	sort(v.begin(), v.end());
	for (int i = 1; i <= 30; i++) {
		auto it = find(v.begin(), v.end(), i);
			if (it == v.end()) {
				cout << i << endl;
			}
	}
	return 0;
}