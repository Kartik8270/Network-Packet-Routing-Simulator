#pragma once
#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
using namespace std;

unordered_map<long long, long long> dijkstraForAll(
    unordered_map<long long, string> &nodeName,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    long long src,
    int filter);

void printDijkstraResult(
    unordered_map<long long, long long> &dist,
    unordered_map<long long, string> &nodeName,
    long long src,
    int filter);

void shortestPathForOne(
    long long from,
    long long to,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    int filter,
    unordered_map<long long, string> &nodeName);
