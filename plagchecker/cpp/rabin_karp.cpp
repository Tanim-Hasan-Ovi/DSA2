#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <functional>
using namespace std;

float compare(const string &text1, const string &text2, int k=5) {
    unordered_set<size_t> hashes1, hashes2;
    hash<string> hasher;
    for (int i = 0; i <= text1.length() - k; ++i) {
        hashes1.insert(hasher(text1.substr(i, k)));
    }
    for (int i = 0; i <= text2.length() - k; ++i) {
        hashes2.insert(hasher(text2.substr(i, k)));
    }
    int common = 0;
    for (auto h : hashes1) {
        if (hashes2.count(h)) common++;
    }
    return (2.0f * common) / (hashes1.size() + hashes2.size()) * 100;
}

int main() {
    ifstream file1("text1.txt"), file2("text2.txt");
    string text1((istreambuf_iterator<char>(file1)), {});
    string text2((istreambuf_iterator<char>(file2)), {});
    cout << compare(text1, text2);
    return 0;
}