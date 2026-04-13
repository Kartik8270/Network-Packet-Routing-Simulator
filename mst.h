#pragma once
#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include "utils.h"
using namespace std;

long long calculateMSTCost(vector<tuple<long long, long long, long long>> &mst);

vector<tuple<long long, long long, long long>> Prim(
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    long long wt,
    bool printEdges,
    unordered_map<long long, string> &nodeName,bool showcost);

vector<tuple<long long, long long, long long>> Kruskal(
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    long long wt,
    bool printEdges,
    unordered_map<long long, string> &nodeName,bool showcost);

unordered_map<long long, vector<tuple<long long, long long>>> AdjacencyListForMst(
    vector<tuple<long long, long long, long long>> mst);

void shortestPathForOneForMst(
    long long from,
    long long to,
    unordered_map<long long, vector<tuple<long long, long long>>> &edges,
    int filter,
    unordered_map<long long, string> &nodeName);

unordered_map<long long, long long> dijkstraForAllForMst(
    int V,
    unordered_map<long long, vector<tuple<long long, long long>>> &edges,
    long long src);

void printDijkstraResultForMst(
    unordered_map<long long, long long> &dist,
    unordered_map<long long, string> &nodeName,
    long long src);
