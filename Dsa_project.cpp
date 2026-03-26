#include <iostream>
#include <vector>
#include<bits/stdc++.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <tuple>
#include <climits>
#include <stack>
#include <queue>
#include <set>
#include <fstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

void display(int V, vector<tuple<long long , long long , long long , long long , bool>> &edges,map<long long,string> &nodeName){
    vector<vector<pair<long,pair<long,long>>>>adj(V+1);
    for(auto it: edges){
        long long u = get<0>(it);
        long long v = get<1>(it);
        long long latency = get<2>(it);
        long long cost = get<3>(it);
        bool status = get<4>(it);
        if(status){
            adj[v].push_back({u,{latency,cost}});
            adj[u].push_back({v,{latency,cost}});
        }
    }
        cout << "\n=========== NETWORK (ADJ LIST) ===========\n";

    for(int i = 1; i <= V; i++){

        cout << "\n" << nodeName[i] << ":\n";

        if(adj[i].empty()){
            cout << "   No connections\n";
            continue;
        }

        for(auto x : adj[i]){
            int neighbor = x.first;
            int latency = x.second.first;
            int cost = x.second.second;

            cout << "   -> " << nodeName[neighbor]
                 << " | Latency: " << latency << " ms"
                 << " | Cost: " << cost << "\n";
        }
    }

    cout << "\n=========================================\n";

}

// Dijkstra's Algorithm
 vector<long long> dijkstraForAll(int V, vector<tuple<long long , long long , long long , long long , bool>> &edges, long long src , int filter) {
        // Code here
         vector <vector <pair<long long , long long>>> vec(V + 1);
         if(filter == 1){
            for(int i = 0 ; i < edges.size() ; i++){
                vec[get<0>(edges[i])].push_back({get<1>(edges[i]), get<2>(edges[i])});
                vec[get<1>(edges[i])].push_back({get<0>(edges[i]), get<2>(edges[i])});
            }
        }
        else if(filter == 2){
             for(int i = 0 ; i < edges.size() ; i++){
                vec[get<0>(edges[i])].push_back({get<1>(edges[i]), get<3>(edges[i])});
                vec[get<1>(edges[i])].push_back({get<0>(edges[i]), get<3>(edges[i])});
            }
        }
        priority_queue <pair<long long , long long> , vector<pair<long long , long long>> , greater<pair<long long , long long>>> h;
        vector <long long> dist(V , LLONG_MAX);
        dist[src] = 0;
        h.push({0 , src});
        while(!h.empty()){
            long long node = h.top().second;
            long long len = h.top().first;
            h.pop();
            if(len > dist[node]) continue;
            for(auto x : vec[node]){
                if(len + x.second < dist[x.first]){
                    dist[x.first] = len + x.second;
                    h.push({len + x.second , x.first});
                }
            }
        }
        for(int i = 0 ; i < V ; i++) if(dist[i] == LLONG_MAX) dist[i] = -1;
        return dist;
    }

    // shortest path in undirected graph from 1 to n
    void shortestPathForOne(long long from ,long long to, vector<tuple<long long , long long , long long ,long long , bool>>& edges , int filter) {
        // Code here
        vector <vector <pair<long long , long long>>> vec(to + 1);
        for(long long i = 0 ; i < edges.size() ; i++){
            long long start = get<0>(edges[i]);
            long long end = get<1>(edges[i]);
            long long latency = get<2>(edges[i]);
            long long cost = get<3>(edges[i]);
            bool status = get<4>(edges[i]);
            if(filter == 1) vec[start].push_back({end , latency});
            else vec[start].push_back({end , cost});
        }
        priority_queue <pair<long long , long long> , vector<pair<long long , long long>> , greater<pair<long long , long long>>> h;
        vector<long long> dist(to + 1 , LLONG_MAX);
        dist[from] = 0;
        h.push({0 , from});
        vector <long long> parent(to + 1);
        for(long long i = 0 ; i <= to ; i++) parent[i] = i;
        while(!h.empty()){
            long long node = h.top().second;
            long long len = h.top().first;
            h.pop();
            for(auto x : vec[node]){
                if(len + x.second < dist[x.first]){
                    parent[x.first] = node;
                    dist[x.first] = len + x.second;
                    h.push({len + x.second , x.first});
                }
            }
        }
        if(dist[to] == LLONG_MAX) cout << "No path exists" << endl;
        vector <int> path;
        int b = to;
        while(parent[b] != b) {
            path.push_back(b);
            b = parent[b];
        }
        path.push_back(from);
        reverse(path.begin() , path.end());
        cout << "Shortest path from " << from << " to " << to << " is: ";
        for(auto x : path) cout << x << " ";
        cout << endl;
    }
int main(){

    ifstream inFile("Data.json");
    if (!inFile.is_open()) {
        cerr << "Error: Could not open file to read!" << endl;
        return 1;
    }

    json networkData;
    inFile >> networkData; // Load all data into memory
    inFile.close();
    long long nodesSize = networkData["nodes"].size();
    long long edgesSize = networkData["links"].size();
    int i = 0;
    vector <pair <string , string>> Nodes(nodesSize + 1);
    map<long long, string> nodeName;
    for(auto &x : networkData["nodes"]){
        string location = x["location"];
        string ip = x["ip_prefix"];
        Nodes[i] = {ip , location};
        i++;
    }
    for(auto &x : networkData["nodes"]){
        long long id = x["id"];
        string location = x["location"];

        nodeName[id] = location;
    }
    vector<tuple<long long , long long , long long , long long , bool>> AdjacencyList;
    for(int i = 0 ; i < edgesSize ; i++){
        long long from = networkData["links"][i]["source"];
        long long to   = networkData["links"][i]["target"];

        long long latency = networkData["links"][i]["latency_ms"];
        long long cost    = networkData["links"][i]["cost_usd"];

        bool status = (networkData["links"][i]["status"] == "active");

        AdjacencyList.push_back({from , to , latency , cost , status});
    }
    
    int n = 0;
    while(n != 7){
        cout << "These are the following features enter your choice" << endl ;
        cout << "1. Optimized path from 1 place to other" << endl;
        cout << "2. Optimized path from one to all others" << endl;
        cout << "3. display graph" << endl;
        cout << "4. Add node" << endl;
        cout << "5. Delete node" << endl; 
        cout << "6. Display MST" << endl;
        cout << "7. Exit" << endl;
        cin >> n;
        switch(n){
            case 1:
                cout << "Which filter you want" << endl;
                cout << "1. Latency" << endl;
                cout << "2. Cost" << endl;
                int filter;
                cin >> filter;
                if(filter == 1){
                    cout << "Enter source and destination(start from 1)" << endl;
                    int from , to;
                    cin >> from >> to;
                    shortestPathForOne(from , to , AdjacencyList , filter);
                }
                else if(filter == 2){
                    cout << "Enter source and destination(start from 1)" << endl;
                    int from , to;
                    cin >> from >> to;
                    shortestPathForOne(from , to , AdjacencyList , filter);
                }
                break;
            case 2:
                cout << "Enter source(start from 1)" << endl;
                cout << "Which filter you want" << endl;
                cout << "1. Latency" << endl;
                cout << "2. Cost" << endl;
                int filter2;
                cin >> filter2;
                if(filter2 == 1){
                    cout << "Enter source(start from 1)" << endl;
                    int src;
                    cin >> src;
                    dijkstraForAll(nodesSize , AdjacencyList , src , filter2);
                }
                else if(filter2 == 2){
                    cout << "Enter source(start from 1)" << endl;
                    int src;
                    cin >> src;
                    dijkstraForAll(nodesSize + 1 , AdjacencyList , src , filter2);
                }
                break;
            case 3:
                cout << "Display graph feature is under construction" << endl;
                display(nodesSize, AdjacencyList, nodeName);
                break;
            case 4:
                cout << "Add node feature is under construction" << endl;
                break;
            case 5:
                cout << "Delete node feature is under construction" << endl;
                break; 
            case 6:
                cout << "Display MST feature is under construction" << endl;
                break; 
            case 7:
                cout << "Exiting..." << endl;
                break; 
            default:
                cout << "Invalid choice" << endl; 
        }
    }

    return 0;
}