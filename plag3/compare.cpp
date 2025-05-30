#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

const int BASE = 256;
const int MOD = 101;

// Rabin-Karp Similarity (symmetric by using min length for normalization)
float rabin_karp_similarity(const string &text1, const string &text2) {
    if (text1.empty() || text2.empty()) return 0.0f;

    vector<int> pattern_lengths = {5, 10, 20};
    float total_score = 0.0f;
    int weight_sum = 0;

    for (int pattern_len : pattern_lengths) {
        if (text1.length() < pattern_len || text2.length() < pattern_len) continue;

        unordered_map<int, int> hashes1;
        int weight = pattern_len;

        // Precompute h value
        int h = 1;
        for (int i = 0; i < pattern_len - 1; i++) h = (h * BASE) % MOD;

        // Hashes for text1
        int hash = 0;
        for (int i = 0; i < pattern_len; i++)
            hash = (BASE * hash + text1[i]) % MOD;
        hashes1[hash]++;

        for (int i = pattern_len; i < text1.length(); i++) {
            hash = (BASE * (hash - text1[i - pattern_len] * h) + text1[i]) % MOD;
            if (hash < 0) hash += MOD;
            hashes1[hash]++;
        }

        // Compare with text2
        int matches = 0;
        hash = 0;
        for (int i = 0; i < pattern_len; i++)
            hash = (BASE * hash + text2[i]) % MOD;
        if (hashes1.count(hash)) matches++;

        for (int i = pattern_len; i < text2.length(); i++) {
            hash = (BASE * (hash - text2[i - pattern_len] * h) + text2[i]) % MOD;
            if (hash < 0) hash += MOD;
            if (hashes1.count(hash)) matches++;
        }

        float score = (float)matches / (min(text1.length(), text2.length()) - pattern_len + 1);
        total_score += score * weight;
        weight_sum += weight;
    }

    return weight_sum > 0 ? (total_score / weight_sum) * 100.0f : 0.0f;
}

// LCS Similarity (symmetric by nature)
float lcs_similarity(const string &X, const string &Y) {
    int m = X.length(), n = Y.length();
    if (m == 0 || n == 0) return 0.0f;

    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= m; i++) {
        int prev = 0;
        for (int j = 1; j <= n; j++) {
            int temp = dp[j];
            if (X[i - 1] == Y[j - 1])
                dp[j] = prev + 1;
            else
                dp[j] = max(dp[j], dp[j - 1]);
            prev = temp;
        }
    }

    int lcs_len = dp[n];
    return (2.0f * lcs_len) / (m + n) * 100.0f;
}

// Combined Symmetric Similarity
float combined_similarity(const string &text1, const string &text2) {
    float rk_sim1 = rabin_karp_similarity(text1, text2);
    float rk_sim2 = rabin_karp_similarity(text2, text1);
    float rk_sim = (rk_sim1 + rk_sim2) / 2.0f;

    float lcs_sim = lcs_similarity(text1, text2);

    // Combine with weighting (e.g., LCS 60%, RK 40%)
    return (lcs_sim * 0.6f + rk_sim * 0.4f);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <file1> <file2>" << endl;
        return 1;
    }

    ifstream file1(argv[1]), file2(argv[2]);
    if (!file1 || !file2) {
        cerr << "Error opening files" << endl;
        return 1;
    }

    string text1((istreambuf_iterator<char>(file1)), {});
    string text2((istreambuf_iterator<char>(file2)), {});

    float similarity = combined_similarity(text1, text2);
    cout << similarity;

    return 0;
}
