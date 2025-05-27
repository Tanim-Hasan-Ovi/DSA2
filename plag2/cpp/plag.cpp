#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

// Rabin-Karp এর জন্য হ্যাশ গণনা
unordered_set<size_t> compute_hashes(const string &text, int k) {
    unordered_set<size_t> hashes;
    hash<string> hasher;
    for (int i = 0; i <= text.length() - k; ++i) {
        hashes.insert(hasher(text.substr(i, k)));
    }
    return hashes;
}

// LCS সিমিলারিটি
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

// দুই মেথডের কম্বিনেশন
float combined_similarity(const string &text1, const string &text2, int k=5) {
    auto hashes1 = compute_hashes(text1, k);
    auto hashes2 = compute_hashes(text2, k);
    int common_hashes = 0;
    for (auto h : hashes1) {
        if (hashes2.count(h)) common_hashes++;
    }
    float rk_score = (2.0f * common_hashes) / (hashes1.size() + hashes2.size()) * 100;
    float lcs_score = lcs_similarity(text1, text2);
    return 0.6f * lcs_score + 0.4f * rk_score;
}

int main() {
    ifstream file1("text1.txt"), file2("text2.txt");
    if (!file1 || !file2) {
        cerr << "Error: File(s) could not be opened." << endl;
        return 1;
    }

    string text1((istreambuf_iterator<char>(file1)), {});
    string text2((istreambuf_iterator<char>(file2)), {});

    float similarity = combined_similarity(text1, text2);
    cout << similarity << endl;  // শুধুমাত্র সংখ্যা আউটপুট
    return 0;
}
