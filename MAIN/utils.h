#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <queue>
#include <unordered_map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

class DSU {
public:
    unordered_map<long long, long long> parent, rank;

    void makeSet(long long v) {
        parent[v] = v;
        rank[v] = 0;
    }

    long long find(long long v) {
        if (v == parent[v]) return v;
        return parent[v] = find(parent[v]);
    }

    void unite(long long a, long long b) {
        a = find(a);
        b = find(b);
        if (a != b) {
            if (rank[a] < rank[b]) swap(a, b);
            parent[b] = a;
            if (rank[a] == rank[b]) rank[a]++;
        }
    }
};

bool validNode(long long id, unordered_map<long long, string> &nodeName);

void display(int V,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    unordered_map<long long, string> &nodeName);

void numDelete(int d,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    unordered_map<long long, string> &nodeName);

void deleteEdges(int d,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    unordered_map<long long, string> &nodeName);
