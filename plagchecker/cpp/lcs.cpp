#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

float lcs_similarity(const string &text1, const string &text2) {
    int m = text1.length(), n = text2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (text1[i-1] == text2[j-1]) dp[i][j] = dp[i-1][j-1] + 1;
            else dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    }
    return (2.0f * dp[m][n]) / (m + n) * 100;
}

int main() {
    ifstream file1("text1.txt"), file2("text2.txt");
    string text1((istreambuf_iterator<char>(file1)), {});
    string text2((istreambuf_iterator<char>(file2)), {});
    cout << lcs_similarity(text1, text2);
    return 0;
}