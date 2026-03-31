#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <tuple>
#include <climits>
#include <stack>
#include <queue>
#include <set>
#include <fstream>
#include <unordered_map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
#define int long long

bool validNode(long long id, unordered_map<long long,string> &nodeName){
    return nodeName.find(id) != nodeName.end();
}

void Prim(unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,long long wt,bool printEdges,unordered_map<long long, string> &nodeName){
    if (adj.empty()) {
        cout << "Graph is empty\n";
        return;
    }
    unordered_map<int,bool>visited;
    int start = adj.begin()->first;
    vector<tuple<int,int,int>>mst;
    int totalCost = 0;
    priority_queue<pair<int,pair<int,int>>,vector<pair<int,pair<int,int>>>,greater<pair<int,pair<int,int>>>>pq;
    pq.push({0,{start,-1}});
    while(!pq.empty()){
        int l = pq.top().first;
        int node = pq.top().second.first;
        int parent = pq.top().second.second;
        pq.pop();
        if(!visited[node]){
            visited[node] = true;
            if(parent!=-1){
                totalCost+=l;
                mst.push_back({node,parent,l});
            }
            for(auto &it:adj[node]){
                long long neighbor = get<0>(it);
                long long weight;
                if(wt==1){
                    weight = get<1>(it);
                }
                else{
                    weight = get<2>(it);
                }
                if(!visited[neighbor]){
                    pq.push({weight,{neighbor,node}});
                }
            }
        }
    }
    if (wt == 1) {
        cout << "\nTotal MST latency: " << totalCost << " ms\n\n";
    } 
    else {
        cout << "\nTotal MST cost: " << totalCost << " USD\n\n";
    }
    if (wt == 1){
            cout << "\n--- Latency Optimized MST ---\n";
        }
    else{
        cout << "\n--- Cost Optimized MST ---\n";
    }

    if (printEdges) {
        
        unordered_map<long long, vector<tuple<long long,long long>>> mstAdj;
        for (auto &e : mst) {
            long long u = get<0>(e); // parent
            long long v = get<1>(e);  // node
            int wei = get<2>(e);

            mstAdj[u].push_back({v,wei});
            mstAdj[v].push_back({u,wei});
        }
        cout << "\nMST Adjacency List (with city names):\n";

        for (auto &y : mstAdj) {
            cout << "\n[" << y.first << "] " << nodeName[y.first] << " :\n";

            for (auto &x : y.second) {
                long long nbr = get<0>(x);
                long long w   = get<1>(x);

                cout << "   -> [" << nbr << "] " << nodeName[nbr]
                    << "  weight: " << w << "\n";
            }
        }
    }
}

// display graph
void display(int V, unordered_map<long long, vector<tuple<long long, long long, long long>>> &adj,
             unordered_map<long long, string> &nodeName) {

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

// dijkstra from one source to all nodes
unordered_map<long long, long long> dijkstraForAll(int V,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    long long src, int filter) {

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

        if (dist.find(node) != dist.end() && len > dist[node]) continue;
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

// print dijkstra results
void printDijkstraResult(unordered_map<long long, long long> &dist,
                         unordered_map<long long, string> &nodeName,
                         long long src, int filter) {
    cout << "\nRouting table from [" << src << "] " << nodeName[src] << "\n";
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

// shortest path from source to one destination
void shortestPathForOne(int V, long long from, long long to,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    int filter, unordered_map<long long, string> &nodeName) {

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

        if (len > dist[node]) continue;
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
    for (int i = 0; i < path.size(); i++) {
        cout << nodeName[path[i]];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\nhops : " << path.size() - 1;
    cout << "\ntotal " << (filter == 1 ? "latency: " : "cost: $") << dist[to]
         << (filter == 1 ? " ms" : "") << "\n";
}

// delete a node and its edges
void numDelete(int d,
    unordered_map<long long, vector<tuple<long long, long long, long long>>> &edges,
    unordered_map<long long, string> &nodeName) {

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
    unordered_map<long long, vector<tuple<long long,long long,long long>>> &adj,
    unordered_map<long long,string> &nodeName) {

    for (int i = 0; i < d; i++) {
        long long u, v;
        cout << "Enter edge (from to): ";
        cin >> u >> v;
        if (nodeName.find(u) == nodeName.end() ||
                    nodeName.find(v) == nodeName.end()) {
                    cout << "Invalid nodes (one or both do not exist)\n";
                    continue;
                }

        // remove v from u's list
        if (adj.find(u) != adj.end()) {
            auto &vec = adj[u];
            vec.erase(remove_if(vec.begin(), vec.end(),
                [v](auto &t) {
                    return get<0>(t) == v;
                }), vec.end());
        }

        // remove u from v's list
        if (adj.find(v) != adj.end()) {
            auto &vec = adj[v];
            vec.erase(remove_if(vec.begin(), vec.end(),
                [u](auto &t) {
                    return get<0>(t) == u;
                }), vec.end());
        }

        cout << "Edge between " << nodeName[u]
             << " and " << nodeName[v] << " deleted\n";
    }
}

int32_t main() {

    ifstream inFile("Data.json");
    if (!inFile.is_open()) {
        cerr << "could not open Data.json\n";
        return 1;
    }
    json networkData;
    inFile >> networkData;
    inFile.close();

    long long nodesSize = networkData["nodes"].size();
    long long edgesSize = networkData["links"].size();
    unordered_map<long long, string> nodeName;

    for (auto &x : networkData["nodes"]) {
        long long id    = x["id"];
        string location = x["location"];
        nodeName[id]    = location;
    }

    unordered_map<long long, vector<tuple<long long, long long, long long>>> AdjacencyList;
    for (int i = 0; i < edgesSize; i++) {
        long long from    = networkData["links"][i]["source"];
        long long to      = networkData["links"][i]["target"];
        long long latency = networkData["links"][i]["latency_ms"];
        long long cost    = networkData["links"][i]["cost_usd"];
        AdjacencyList[from].push_back({to,   latency, cost});
        AdjacencyList[to].push_back({from, latency, cost});
    }

    cout << "loaded " << nodesSize << " nodes, " << edgesSize << " links\n";

    int n = 0;
    while (n != 12) {
        cout << "\n--- Menu ---\n";
        cout << "1. shortest path (one to one)\n";
        cout << "2. shortest path (one to all)\n";
        cout << "3. display graph\n";
        cout << "4. add edges\n";
        cout << "5. add new node\n";
        cout << "6. delete node\n";
        cout << "7. Delete edges\n";
        cout << "8. MST using Prim's Algo\n";
        cout <<"9. MST using Kruskal's Algo\n";
        cout<< "10. Kruskal vs Prim\n";
        cout << "11. MST vs Djistra\n";
        cout<< "12. exit\n";
        cout << "choice: ";
        cin >> n;

        switch (n) {

            case 1: {
                cout << "filter (1=latency, 2=cost): ";
                int filter; cin >> filter;
                cout << "source id: ";      long long from; cin >> from;
                cout << "destination id: "; long long to;   cin >> to;
                if (!validNode(from,nodeName) || !validNode(to,nodeName)) {
                    cout << "Invalid source or destination\n";
                    break;
                }
                shortestPathForOne(nodesSize, from, to, AdjacencyList, filter, nodeName);
                break;
            }

            case 2: {
                cout << "filter (1=latency, 2=cost): ";
                int filter2; cin >> filter2;
                cout << "source id: "; int src; cin >> src;
                if (!validNode(src,nodeName)) {
                    cout << "Invalid source node\n";
                    break;
                }
                unordered_map<long long, long long> dist = dijkstraForAll(nodesSize, AdjacencyList, src, filter2);
                printDijkstraResult(dist, nodeName, src, filter2);
                break;
            }

            case 3: {
                display(nodesSize, AdjacencyList, nodeName);
                break;
            }

            case 4: {
                cout << "how many edges: "; int m; cin >> m;
                for (int i = 0; i < m; i++) {
                    cout << "from: ";    long long from;    cin >> from;
                    cout << "to: ";      long long to;      cin >> to;
                    cout << "latency: "; long long latency; cin >> latency;
                    cout << "cost: ";    long long cost;    cin >> cost;
                    if (!validNode(from,nodeName) || !validNode(to,nodeName)) {
                        cout << "Invalid nodes\n";
                        continue;
                    }
                    AdjacencyList[from].push_back({to,   latency, cost});
                    AdjacencyList[to].push_back({from, latency, cost});
                }
                break;
            }

            case 5: {
                cout << "how many nodes: "; int k; cin >> k;
                for (int i = 0; i < k; i++) {
                    cout << "id: ";       long long id;    cin >> id;
                    cout << "location: "; string location; cin >> location;
                    cout << "ip: ";       string ip;       cin >> ip;
                    nodeName[id] = location;
                    nodesSize++;
                }
                break;
            }

            case 6: {
                cout << "how many nodes to delete: "; 
                int d; cin >> d;
                numDelete(d, AdjacencyList, nodeName);
                break;
            }

            case 7: {
                cout << "How many edges to delete: ";
                int d;
                cin >> d;
                deleteEdges(d, AdjacencyList, nodeName);
                break;
            }

            case 8: {
                long long wt;
                int choice;

                cout << "Press 1 for Latency optimized network\n";
                cout << "Press 2 for Cost optimized network\n";
                cin >> wt;

                cout << "Press 1 to print Only total cost\n";
                cout << "Press 2 to show full MST\n";
                cin >> choice;

                bool printEdges = (choice == 2);

                Prim(AdjacencyList, wt, printEdges,nodeName);
                break;
            }
            case 9:{
                //MST using Kruskal's Algo\n;
                break;
            }
            case 10:{
                //Kruskal vs Prim\n";
                break;
            }
            case 11:{
                //MST vs Djistra\n";
                break;
            }
            case 12: {
                cout << "bye\n";
                break;
            }


            default:
                cout << "invalid choice\n";
        }
    }

    return 0;
}