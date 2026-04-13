#include "mst.h"

long long calculateMSTCost(vector<tuple<long long, long long, long long>> &mst) {
    long long total = 0;
    for (auto &e : mst) total += get<2>(e);
    return total;
}

vector<tuple<long long, long long, long long>> Prim(
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    long long wt,
    bool printEdges,
    unordered_map<long long, string> &nodeName,bool showcost) {

    if (adj.empty()) {
        cout << "graph is empty\n";
        return {};
    }

    unordered_map<long long, bool> visited;
    long long start = adj.begin()->first;
    vector<tuple<long long, long long, long long>> mst;
    long long totalCost = 0;

    priority_queue<pair<long long, pair<long long, long long>>,
                   vector<pair<long long, pair<long long, long long>>>,
                   greater<pair<long long, pair<long long, long long>>>> pq;

    pq.push({0, {start, -1}});

    while (!pq.empty()) {
        long long l    = pq.top().first;
        long long node = pq.top().second.first;
        long long par  = pq.top().second.second;
        pq.pop();

        if (visited.find(node) == visited.end() || !visited[node]) {
            visited[node] = true;
            if (par != -1) {
                totalCost += l;
                mst.push_back({node, par, l});
            }
            if (adj.find(node) == adj.end()) continue;
            for (auto &it : adj[node]) {
                long long neighbor = get<0>(it);
                long long weight   = (wt == 1) ? get<1>(it) : get<2>(it);
                if (visited.find(neighbor) == visited.end() || !visited[neighbor]) {
                    pq.push({weight, {neighbor, node}});
                }
            }
        }
    }

    if (nodeName.size() > 1 && mst.size() != nodeName.size() - 1) {
        cout << "warning: graph is disconnected, MST does not cover all nodes\n";
        cout << "nodes covered: " << mst.size() + 1 << " out of " << nodeName.size() << "\n";
    }
    if(showcost){
        if (wt == 1)
            cout << "\n--- Latency Optimized MST (Prim) ---\n";
        else
            cout << "\n--- Cost Optimized MST (Prim) ---\n";

        if (wt == 1)
            cout << "total MST latency: " << totalCost << " ms\n";
        else
            cout << "total MST cost: $" << totalCost << "\n";

    }
    
    if (printEdges) {
        unordered_map<long long, vector<tuple<long long, long long>>> mstAdj;
        for (auto &e : mst) {
            long long u   = get<0>(e);
            long long v   = get<1>(e);
            long long wei = get<2>(e);
            mstAdj[u].push_back({v, wei});
            mstAdj[v].push_back({u, wei});
        }
        cout << "\nMST adjacency list:\n";
        for (auto &y : mstAdj) {
            cout << "\n[" << y.first << "] " << nodeName[y.first] << " :\n";
            for (auto &x : y.second) {
                cout << "   -> [" << get<0>(x) << "] " << nodeName[get<0>(x)]
                     << "  weight: " << get<1>(x) << "\n";
            }
        }
    }

    return mst;
}

vector<tuple<long long, long long, long long>> Kruskal(
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    long long wt,
    bool printEdges,
    unordered_map<long long, string> &nodeName,bool showcost) {

    if (adj.empty()) {
        cout << "graph is empty\n";
        return {};
    }

    vector<tuple<long long, long long, long long>> edges;
    for (auto &x : adj) {
        long long u = x.first;
        for (auto &e : x.second) {
            long long v = get<0>(e);
            long long w = (wt == 1) ? get<1>(e) : get<2>(e);
            if (u < v) edges.push_back({w, u, v});
        }
    }

    sort(edges.begin(), edges.end());

    DSU dsu;
    for (auto &x : nodeName) dsu.makeSet(x.first);

    vector<tuple<long long, long long, long long>> mst;
    long long totalCost = 0;

    for (auto &e : edges) {
        long long w = get<0>(e);
        long long u = get<1>(e);
        long long v = get<2>(e);
        if (dsu.find(u) != dsu.find(v)) {
            dsu.unite(u, v);
            mst.push_back({u, v, w});
            totalCost += w;
        }
    }

    if (nodeName.size() > 1 && mst.size() != nodeName.size() - 1) {
        cout << "warning: graph is disconnected, MST does not cover all nodes\n";
        cout << "nodes covered: " << mst.size() + 1 << " out of " << nodeName.size() << "\n";
    }
    if(showcost){
        if (wt == 1)
            cout << "\n--- Latency Optimized MST (Kruskal) ---\n";
        else
            cout << "\n--- Cost Optimized MST (Kruskal) ---\n";

        cout << "total MST " << (wt == 1 ? "latency: " : "cost: $")
            << totalCost << (wt == 1 ? " ms\n" : "\n");
    }
    

    if (printEdges) {
        for (auto &e : mst) {
            cout << nodeName[get<0>(e)] << " <--> "
                 << nodeName[get<1>(e)]
                 << "  weight: " << get<2>(e) << "\n";
        }
    }

    return mst;
}

unordered_map<long long, vector<tuple<long long, long long>>> AdjacencyListForMst(
    vector<tuple<long long, long long, long long>> mst) {

    unordered_map<long long, vector<tuple<long long, long long>>> mstAdj;
    for (auto &e : mst) {
        long long u   = get<0>(e);
        long long v   = get<1>(e);
        long long wei = get<2>(e);
        mstAdj[u].push_back({v, wei});
        mstAdj[v].push_back({u, wei});
    }
    return mstAdj;
}

void shortestPathForOneForMst(
    long long from,
    long long to,
    unordered_map<long long, vector<tuple<long long, long long>>> &edges,
    int filter,
    unordered_map<long long, string> &nodeName){

    if (edges.empty()) {
        cout << "MST is empty, run MST first\n";
        return;
    }

    if (edges.find(from) == edges.end()) {
        cout << "source node not in MST\n";
        return;
    }
    if (edges.find(to) == edges.end()) {
        cout << "destination node not in MST\n";
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
            long long neighbor = get<0>(x);
            long long weight   = get<1>(x);
            if (dist.find(neighbor) == dist.end() || len + weight < dist[neighbor]) {
                dist[neighbor]   = len + weight;
                parent[neighbor] = node;
                h.push({dist[neighbor], neighbor});
            }
        }
    }

    if (dist.find(to) == dist.end()) {
        cout << "no path found in MST\n";
        return;
    }

    vector<long long> path;
    long long cur = to;
    while (cur != -1) {
        path.push_back(cur);
        cur = parent[cur];
    }
    reverse(path.begin(), path.end());

    cout << "\nMST path: ";
    for (int i = 0; i < (int)path.size(); i++) {
        cout << nodeName[path[i]];
        if (i + 1 < (int)path.size()) cout << " -> ";
    }
    cout << "\nhops: " << path.size() - 1;
    cout << "\n--- MST path (optimized for "
     << (filter == 1 ? "latency" : "cost")
     << ", not guaranteed shortest) ---";
    cout << "\ntotal " << (filter == 1 ? "latency: " : "cost: $") << dist[to]
     << (filter == 1 ? " ms" : "") << "\n";
}

unordered_map<long long, long long> dijkstraForAllForMst(
    int V,
    unordered_map<long long, vector<tuple<long long, long long>>> &edges,
    long long src) {

    if (edges.empty()) {
        cout << "MST is empty, run MST first\n";
        return {};
    }
    if (edges.find(src) == edges.end()) {
        cout << "source node not in MST\n";
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
            long long weight = get<1>(x);
            if (dist.find(v) == dist.end() || len + weight < dist[v]) {
                dist[v] = len + weight;
                h.push({dist[v], v});
            }
        }
    }
    return dist;
}

void printDijkstraResultForMst(
    unordered_map<long long, long long> &dist,
    unordered_map<long long, string> &nodeName,
    long long src) {

    if (dist.empty()) {
        cout << "no results\n";
        return;
    }
    cout << "\nMST routing table from [" << src << "] " << nodeName[src] << "\n";
    cout << "--------------------------------------------\n";
    for (auto &y : nodeName) {
        long long id = y.first;
        if (id == src) continue;
        cout << "[" << id << "] " << y.second << " : ";
        if (dist.find(id) == dist.end())
            cout << "unreachable\n";
        else
            cout << dist[id] << "\n";
    }
    cout << "--------------------------------------------\n";
}
