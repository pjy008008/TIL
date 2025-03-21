#include <iostream>
#include <vector>
// #include <algorithm>
// using namespace std;
//
// int main() {
//	int n, m, k;
//	int x;
//	int sum = 0;
//	int count = 0;
//	vector<int> v;
//	cin >> n >> m >> k;
//	for (int i = 0; i < n; i++) {
//		cin >> x;
//		v.push_back(x);
//	}
//	sort(v.begin(), v.end());
//	if (k < m) {
//		sum = m / (k + 1) * (v.back() * k + v[n - 2]);
//		count = m % (k + 1);
//		sum += count * v.back();
//		cout << sum;
//	}
//	else {
//		sum = v.back() * m;
//		cout << sum;
//	}
//	return 0;
// }

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    int n, m, k;
    int x;
    int sum = 0;
    int count = 0;
    vector<int> v;
    cin >> n >> m >> k;
    for (int i = 0; i < n; i++)
    {
        cin >> x;
        v.push_back(x);
    }
    sort(v.begin(), v.end());
    for (int i = 0; i < m; i++)
    {
        if (count == k)
        {
            sum += v[n - 2];
            count=0;
        }
        else
        {
            sum += v.back();
        }
        count++;
    }
    printf("%d\n", sum);
    return 0; 
    //18 5 18 5
}