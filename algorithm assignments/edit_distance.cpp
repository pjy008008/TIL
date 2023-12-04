#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int getMin(int a, int b, int c)
{
    return min(min(a, b), c);
}

int editDistance(const string &str1, const string &str2)
{
    int m = str1.length();
    int n = str2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            if (i == 0)
            {
                dp[i][j] = j;
            }
            else if (j == 0)
            {
                dp[i][j] = i;
            }
            else if (str1[i - 1] == str2[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = 1 + getMin(dp[i - 1][j],       // 삭제
                                      dp[i][j - 1],       // 삽입
                                      dp[i - 1][j - 1]);  // 교체
            }
        }
    }
    for (int i = 0; i <= m; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            cout.width(3);
            cout << dp[i][j];
        }
        cout << endl;
    }

    return dp[m][n];
}

int main()
{

    string str1, str2;
    str1 = "He cannot be reliable";
    str2 = "He can be unreliable";

    int distance = editDistance(str1, str2);
    cout << "편집 거리: " << distance << endl;

    return 0;
}
