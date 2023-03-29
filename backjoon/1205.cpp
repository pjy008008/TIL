#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
int main() {
	int n, t, p, x;
	int ranking = 1;
	vector<int> v;
	cin >> n >> t >> p;
	for (int i = 0; i < n; i++) {
		cin >> x;
		v.push_back(x);
	}
	if (n != 0) {
		for (int i = 0; i < n; i++) {
			if (v[i] <= t) {
				ranking = i + 1;
				break;
			}
			else if (i == n - 1) {
				ranking = n + 1;
			}
			else {
				continue;
			}
		}
	}
	else {
		cout << 1;
		return 0;
	}
	if (n>=p && v[n - 1] == t) {
		cout << -1;
	}
	else if (ranking > p) {
		cout << -1;
	}
	else {
		cout << ranking;
	}
	return 0;
}