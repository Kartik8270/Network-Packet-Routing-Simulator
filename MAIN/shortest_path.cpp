#include "shortest_path.h"
#include <algorithm>

unordered_map<long long, long long> dijkstraForAll(
    unordered_map<long long, string> &nodeName,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    long long src,
    int filter) {

    if (edges.empty()) {
        cout << "graph is empty\n";
        return {};
    }
    if (edges.find(src) == edges.end()) {
        cout << "source node not found\n";
        return {};
    }

    priority_queue<pair<long long, long long>,
                   vector<pair<long long, long long>>,
                   greater<pair<long long, long long>>> h;

    unordered_map<long long, long long> dist;
    dist[src] = 0;
    h.push({0, src});

    while (!h.empty()) {
        long long len  = h.top().first;
        long long node = h.top().second;
        h.pop();

        if (dist.count(node) && len > dist[node]) continue;
        if (edges.find(node) == edges.end()) continue;

        for (auto &x : edges[node]) {
            long long v      = get<0>(x);
            long long weight = (filter == 1) ? get<1>(x) : get<2>(x);
            if (dist.find(v) == dist.end() || len + weight < dist[v]) {
                dist[v] = len + weight;
                h.push({dist[v], v});
            }
        }
    }
    return dist;
}

void printDijkstraResult(
    unordered_map<long long, long long> &dist,
    unordered_map<long long, string> &nodeName,
    long long src,
    int filter) {

    if (dist.empty()) {
        cout << "no results\n";
        return;
    }
    cout << "\nrouting table from [" << src << "] " << nodeName[src] << "\n";
    cout << "filter: " << (filter == 1 ? "latency" : "cost") << "\n";
    cout << "--------------------------------------------\n";
    for (auto &y : nodeName) {
        long long id = y.first;
        if (id == src) continue;
        cout << "[" << id << "] " << y.second << " : ";
        if (dist.find(id) == dist.end())
            cout << "unreachable\n";
        else
            cout << dist[id] << (filter == 1 ? " ms" : " USD") << "\n";
    }
    cout << "--------------------------------------------\n";
}

void shortestPathForOne(
    long long from,
    long long to,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    int filter,
    unordered_map<long long, string> &nodeName) {

    if (edges.empty()) {
        cout << "graph is empty\n";
        return;
    }

    if (edges.find(from) == edges.end()) {
        cout << "source node has no edges\n";
        return;
    }

    priority_queue<pair<long long, long long>,
                   vector<pair<long long, long long>>,
                   greater<pair<long long, long long>>> h;

    unordered_map<long long, long long> dist;
    unordered_map<long long, long long> parent;
    dist[from]   = 0;
    parent[from] = -1;
    h.push({0, from});

    while (!h.empty()) {
        long long len  = h.top().first;
        long long node = h.top().second;
        h.pop();

        if (dist.count(node) && len > dist[node]) continue;
        if (edges.find(node) == edges.end()) continue;

        for (auto &x : edges[node]) {
            long long weight   = (filter == 1) ? get<1>(x) : get<2>(x);
            long long neighbor = get<0>(x);
            if (dist.find(neighbor) == dist.end() || len + weight < dist[neighbor]) {
                parent[neighbor] = node;
                dist[neighbor]   = len + weight;
                h.push({dist[neighbor], neighbor});
            }
        }
    }

    if (dist.find(to) == dist.end()) {
        cout << "no path found\n";
        return;
    }

    vector<long long> path;
    long long b = to;
    while (b != -1) {
        path.push_back(b);
        b = parent[b];
    }
    reverse(path.begin(), path.end());

    cout << "\npath: ";
    for (int i = 0; i < (int)path.size(); i++) {
        cout << nodeName[path[i]];
        if (i + 1 < (int)path.size()) cout << " -> ";
    }
    cout << "\nhops: " << path.size() - 1;
    cout << "\ntotal " << (filter == 1 ? "latency: " : "cost: $") << dist[to]
         << (filter == 1 ? " ms" : "") << "\n";
}
