#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
    int m, n;
    int result = 0;
    cin >> n >> m;
    for (int i = 0; i < n; i++)
    {
        int min_value = 10001;
        for (int j = 0; j < m; j++)
        {
            int x;
            cin >> x;
            min_value = min(min_value, x);
        }
        result = max(min_value, result);
    }
    cout << result << endl;
    return 0;
}

// #include <iostream>
// #include <vector>
// #include <algorithm>
// using namespace std;

// int main()
// {
//     int n, m;
//     int x;
//     int rowmin = 0;
//     vector<int> v;
//     cin >> n >> m;

//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < m; j++)
//         {
//             cin >> x;
//             v.push_back(x);
//         }
//         sort(v.begin(), v.end());
//         if (v[0] > rowmin)
//         {
//             rowmin = v[0];
//         }
//         v.clear();
//     }
//     cout << rowmin << endl;
//     return 0;
// }