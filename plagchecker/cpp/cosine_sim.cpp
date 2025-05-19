#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <unordered_map>
using namespace std;

float cosine_similarity(const string &text1, const string &text2) {
    unordered_map<string, int> freq1, freq2;
    stringstream ss1(text1), ss2(text2);
    string word;
    
    // Count word frequencies
    while (ss1 >> word) freq1[word]++;
    while (ss2 >> word) freq2[word]++;
    
    // Calculate dot product and magnitudes
    float dot = 0, mag1 = 0, mag2 = 0;
    
    // C++11 compatible iteration (instead of C++17 structured bindings)
    for (const auto &pair1 : freq1) {
        dot += pair1.second * freq2[pair1.first];
        mag1 += pair1.second * pair1.second;
    }
    for (const auto &pair2 : freq2) {
        mag2 += pair2.second * pair2.second;
    }
    
    // Avoid division by zero
    if (mag1 == 0 || mag2 == 0) return 0.0f;
    
    return (dot / (sqrt(mag1) * sqrt(mag2))) * 100;
}

int main() {
    ifstream file1("text1.txt"), file2("text2.txt");
    if (!file1.is_open() || !file2.is_open()) {
        cerr << "Error opening files!" << endl;
        return 1;
    }
    
    string text1((istreambuf_iterator<char>(file1)), {});
    string text2((istreambuf_iterator<char>(file2)), {});
    
    cout << cosine_similarity(text1, text2);
    return 0;
}