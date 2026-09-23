#include <vector>
#include <cmath>
#include <utility>
#include <random>
#include <iostream>
#include <fstream>
#include <unordered_set>

class BloomFilter {
    public:
    BloomFilter(int n, int c, int k) : n(n), c(c), k(k), m(c*n), table(m, false), hash_data_1(k, {0,0}), hash_data_2(k,0){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, p-1);
        for (int i = 0; i < k; i++){
            hash_data_1[i] = {dis(gen), dis(gen)};
            hash_data_2[i] = dis(gen);
        }
    }

    void add1(int x){
        for (int i = 0; i < k; i++){
            uint64_t h = ((hash_data_1[i].first * x + hash_data_1[i].second)%p)%m;
            table[h] = true;
        }
    }

    bool contains1(int x){
        for (int i = 0; i < k; i++){
            uint64_t h = ((hash_data_1[i].first * x + hash_data_1[i].second)%p)%m;
            if (table[h] == false){
                return false;
            }
        }
        return true;
    }

    void add2(int x){
        for (int i = 0; i < k; i++){
            std::uniform_int_distribution<> dis(0, m-1);
            std::mt19937 gen(hash_data_2[i] + x);
            uint64_t h = dis(gen);
            table[h] = true;
        }
    }

    bool contains2(int x){
        for (int i = 0; i < k; i++){
            std::uniform_int_distribution<> dis(0, m-1);
            std::mt19937 gen(hash_data_2[i] + x);
            uint64_t h = dis(gen);
            if (table[h] == false){
                return false;
            }
        }
        return true;
    }

    void update(int c, int k){
        this->c = c;
        this->k = k;
        this->m = c*n;
        table.clear();
        table.resize(m, false);
        hash_data_1.clear();
        hash_data_1.resize(m, {0,0});
        hash_data_2.clear();
        hash_data_2.resize(m, 0);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, p-1);
        for (int i = 0; i < k; i++){
            hash_data_1[i] = {dis(gen), dis(gen)};
            hash_data_2[i] = dis(gen);
        }
    }

    private:
    int n;
    int c;
    int m;
    int k;
    unsigned int N = pow(2, 31) - 1;
    unsigned int p = pow(2, 31) - 1;
    std::vector<std::pair<uint64_t,uint64_t>> hash_data_1;
    std::vector<uint64_t> hash_data_2;
    std::vector<bool> table;
};

class hash{
    public:
    hash(int n, int c) : n(n), c(c){
        m = n*c;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, p-1);
        a = dis(gen);
        b = dis(gen);
        s = dis(gen);
    }

    uint64_t hash1(int x){
        uint64_t h = ((a * x + b)%p)%m;
        return h;
    }

    uint64_t hash2(int x){
        std::uniform_int_distribution<> dis(0, m-1);
        std::mt19937 gen(s + x);
        uint64_t h = dis(gen);
        return h;
    }

    private:
    int n;
    int c;
    int m;
    uint64_t a;
    uint64_t b;
    uint64_t s;
    unsigned int N = pow(2, 31) - 1;
    unsigned int p = pow(2, 31) - 1;
};

int main(){
    std::ofstream outputFile("hashData.txt");
    unsigned int N = pow(2, 31) - 1;
    int n = 1000;
    int c = 5;
    hash h(n,c);
    int hd1 [n*c] = {};
    int hd2 [n*c] = {};
    uint64_t inputs [n];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, N-1);

    for (int i = 0; i < n; i++){
        inputs[i] = i;
    }

    outputFile << "Hash Function 1: \n";

    for (int i = 0; i < n; i++){
        outputFile << h.hash1(inputs[i]) << std::endl;
        hd1[h.hash1(inputs[i])]++;
    }

    outputFile << "Hash Function 2: \n";

    for (int i = 0; i < n; i++){
        outputFile << h.hash2(inputs[i]) << std::endl;
        hd2[h.hash2(inputs[i])]++;
    }

    int max1 = 0, max2 = 0;

    for (int i = 0; i < c*n; i++){
        if (hd1[i] > max1) {max1 = hd1[i];}
        if (hd2[i] > max2) {max2 = hd2[i];}
    }

    outputFile << "max1: " << max1 << " max2: " << max2;

    outputFile.close();
    
    std::ofstream outputFile("bloomFilterData1.txt");
    int n = 100000;
    int c = 5;
    int k = round(c * log(2) - 2);
    int m = n*c;
    unsigned int N = pow(2, 31) - 1;
    BloomFilter b1(n, c, k);
    BloomFilter b2(n, c, k);

    uint64_t inputs [n];
    uint64_t testSet [n];
    std::unordered_set<uint64_t> groundTruth;

    int fp1 = 0;
    int fp2 = 0;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, N-1);

    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;

    for (int i = 0; i < n; i++){
        inputs[i] = dis(gen);
        testSet[i] = dis(gen);
        groundTruth.emplace(inputs[i]);
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }

    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2))-1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;
    
    k = round(c*log(2));
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2))+1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2) + 2);
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;

    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    c = 10;
    k = round(c * log(2) - 2);
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;

    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2)) - 1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2));
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2))+1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2) + 2);
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    c = 15;
    k = round(c * log(2) - 2);
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2)) - 1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2));
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2))+1;
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    k = round(c*log(2) + 2);
    b1.update(c,k);
    b2.update(c,k);
    fp1 = 0;
    fp2 = 0;
    
    outputFile << "c = " << c << ", k = " << k << ":"<< std::endl;
    for (int i = 0; i < n; i++){
        b1.add1(inputs[i]);
        b2.add2(inputs[i]);
    }
    for (int i = 0; i < n; i++){
        bool c1 = b1.contains1(testSet[i]);
        bool c2 = b2.contains2(testSet[i]);
        bool c = groundTruth.find(testSet[i]) != groundTruth.end();
        if (c != c1) {fp1++;}
        if (c != c2) {fp2++;}
    }

    outputFile << "false positives for hash1: " << fp1 << " false positives for hash2: " << fp2 << std::endl;

    outputFile.close();
    return 0;
}