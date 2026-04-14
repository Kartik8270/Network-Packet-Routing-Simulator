#include "utils.h"
using namespace std;

bool validNode(long long id, unordered_map<long long, string> &nodeName) {
    return nodeName.find(id) != nodeName.end();
}

void display(int V,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    unordered_map<long long, string> &nodeName) {

    if (adj.empty()) {
        cout << "graph is empty\n";
        return;
    }
    cout << "\n--- Network Adjacency List ---\n";
    for (auto &y : nodeName) {
        long long node = y.first;
        cout << "\n[" << node << "] " << y.second << " :\n";
        if (adj.find(node) == adj.end() || adj[node].empty()) {
            cout << "   no connections\n";
            continue;
        }
        for (auto &x : adj[node]) {
            long long neighbor = get<0>(x);
            long long latency  = get<1>(x);
            long long cost     = get<2>(x);
            cout << "   -> [" << neighbor << "] " << nodeName[neighbor]
                 << "  latency: " << latency << "ms"
                 << "  cost: $" << cost << "\n";
        }
    }
    cout << "\ntotal nodes: " << nodeName.size() << "\n";
}

void numDelete(int d,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    unordered_map<long long, string> &nodeName) {

        if (edges.empty()) {
            cout << "graph is empty\n";
            return;
        }
    for (int i = 0; i < d; i++) {
        cout << "enter node id to delete: ";
        long long id;
        cin >> id;

        if (nodeName.find(id) == nodeName.end()) {
            cout << "node not found\n";
            continue;
        }

        string name = nodeName[id];
        nodeName.erase(id);
        edges.erase(id);
        
        for (auto &x : edges) {
            auto &neighbors = x.second;
            neighbors.erase(
                remove_if(neighbors.begin(), neighbors.end(),
                    [id](const tuple<long long, long long, long long> &edge) {
                        return get<0>(edge) == id;
                    }),
                neighbors.end()
            );
        }
        cout << "node " << name << " deleted\n";
    }
}

void deleteEdges(int d,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
    unordered_map<long long, string> &nodeName) {

    for (int i = 0; i < d; i++) {
        long long u, v;
        cout << "enter edge (from to): ";
        cin >> u >> v;

        if (nodeName.find(u) == nodeName.end() || nodeName.find(v) == nodeName.end()) {
            cout << "invalid nodes\n";
            continue;
        }

        bool exists = false;
        if (adj.find(u) != adj.end()) {
            for (auto &e : adj[u]) {
                if (get<0>(e) == v) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            cout << "edge does not exist\n";
            continue;
        }

        auto &vec1 = adj[u];
        vec1.erase(remove_if(vec1.begin(), vec1.end(),
            [v](auto &t) { return get<0>(t) == v; }), vec1.end());

        auto &vec2 = adj[v];
        vec2.erase(remove_if(vec2.begin(), vec2.end(),
            [u](auto &t) { return get<0>(t) == u; }), vec2.end());

        cout << "edge between " << nodeName[u] << " and " << nodeName[v] << " deleted\n";
    }
}
