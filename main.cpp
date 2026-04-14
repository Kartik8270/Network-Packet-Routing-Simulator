#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <fstream>
#include <string>
#include "json.hpp"
#include "utils.h"
#include "shortest_path.h"
#include "mst.h"
using namespace std;
using json = nlohmann::json;

void loadData(json &networkData, unordered_map<long long, string> &nodeName, 
              unordered_map<long long, vector<tuple<long long, long long, long long>>> &AdjacencyList,
              long long &nodesSize, long long &edgesSize) {
    ifstream inFile("Data.json");
    if (!inFile.is_open()) {
        cerr << "could not open Data.json\n";
        exit(1);
    }
    inFile >> networkData;
    inFile.close();

    nodesSize = networkData["nodes"].size();
    edgesSize = networkData["links"].size();

    for (auto &x : networkData["nodes"]) {
        long long id    = x["id"];
        string location = x["location"];
        nodeName[id]    = location;
    }

    for (int i = 0; i < edgesSize; i++) {
        long long from    = networkData["links"][i]["source"];
        long long to      = networkData["links"][i]["target"];
        long long latency = networkData["links"][i]["latency_ms"];
        long long cost    = networkData["links"][i]["cost_usd"];
        AdjacencyList[from].push_back({to,   latency, cost});
        AdjacencyList[to].push_back({from, latency, cost});
    }
}

int main(int argc, char* argv[]) {
    
    // Load data
    json networkData;
    unordered_map<long long, string> nodeName;
    unordered_map<long long, vector<tuple<long long, long long, long long>>> AdjacencyList;
    long long nodesSize, edgesSize;
    
    loadData(networkData, nodeName, AdjacencyList, nodesSize, edgesSize);
    vector<tuple<long long, long long, long long>> mst;

    // Command-line mode
    if (argc > 1) {
        string mode = argv[1];
        
        if (mode == "shortest" && argc == 5) {
            long long from = stoll(argv[2]);
            long long to = stoll(argv[3]);
            int filter = stoi(argv[4]);
            
            if (!validNode(from, nodeName) || !validNode(to, nodeName)) {
                cerr << "invalid source or destination\n";
                return 1;
            }
            if (from == to) {
                cerr << "source and destination are the same\n";
                return 1;
            }
            shortestPathForOne(from, to, AdjacencyList, filter, nodeName);
        }
        else if (mode == "dijkstra_all" && argc == 4) {
            long long src = stoll(argv[2]);
            int filter = stoi(argv[3]);
            
            if (!validNode(src, nodeName)) {
                cerr << "invalid source node\n";
                return 1;
            }
            unordered_map<long long, long long> dist = dijkstraForAll(nodeName, AdjacencyList, src, filter);
            printDijkstraResult(dist, nodeName, src, filter);
        }
        else if (mode == "mst_prim" && argc == 3) {
            int filter = stoi(argv[2]);
            mst = Prim(AdjacencyList, filter, true, nodeName, true);
        }
        else if (mode == "mst_kruskal" && argc == 3) {
            int filter = stoi(argv[2]);
            mst = Kruskal(AdjacencyList, filter, true, nodeName, true);
        }
        else {
            cerr << "Usage:\n";
            cerr << "  " << argv[0] << " shortest <from> <to> <filter>\n";
            cerr << "  " << argv[0] << " dijkstra_all <src> <filter>\n";
            cerr << "  " << argv[0] << " mst_prim <filter>\n";
            cerr << "  " << argv[0] << " mst_kruskal <filter>\n";
            return 1;
        }
        return 0;
    }

    // CLI mode (original functionality)
    cout << "loaded " << nodesSize << " nodes, " << edgesSize << " links\n";

    int n = 0;
    while (n != 13) {
        cout << "\n--- Menu ---\n";
        cout << "1.  shortest path (one to one)\n";
        cout << "2.  shortest path (one to all)\n";
        cout << "3.  display graph\n";
        cout << "4.  add edges\n";
        cout << "5.  add new node\n";
        cout << "6.  delete node\n";
        cout << "7.  delete edges\n";
        cout << "8.  MST using Prim's algo\n";
        cout << "9.  MST using Kruskal's algo\n";
        cout << "10. Kruskal vs Prim\n";
        cout << "11. MST vs Dijkstra\n";
        cout << "12. MST structure\n";
        cout << "13. exit\n";
        cout << "choice: ";
        cin >> n;

        switch (n) {

            case 1: {
                cout << "filter (1=latency, 2=cost): ";
                int filter; cin >> filter;
                cout << "source id: ";      long long from; cin >> from;
                cout << "destination id: "; long long to;   cin >> to;
                if (!validNode(from, nodeName) || !validNode(to, nodeName)) {
                    cout << "invalid source or destination\n";
                    break;
                }
                if (from == to) {
                    cout << "source and destination are the same\n";
                    break;
                }
                shortestPathForOne(from, to, AdjacencyList, filter, nodeName);
                break;
            }

            case 2: {
                cout << "filter (1=latency, 2=cost): ";
                int filter2; cin >> filter2;
                cout << "source id: "; long long src; cin >> src;
                if (!validNode(src, nodeName)) {
                    cout << "invalid source node\n";
                    break;
                }
                unordered_map<long long, long long> dist = dijkstraForAll(nodeName, AdjacencyList, src, filter2);
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
                    if (!validNode(from, nodeName) || !validNode(to, nodeName)) {
                        cout << "invalid nodes\n";
                        continue;
                    }
                    if (from == to) {
                        cout << "self-loops not allowed\n";
                        continue;
                    }
                    bool exists = false;
                    if (AdjacencyList.find(from) != AdjacencyList.end()) {
                        for (auto &e : AdjacencyList[from]) {
                            if (get<0>(e) == to) {
                                exists = true;
                                break;
                            }
                        }
                    }
                    if (exists) {
                        cout << "edge already exists between " << nodeName[from]
                             << " and " << nodeName[to] << "\n";
                    }
                    else {
                        AdjacencyList[from].push_back({to,   latency, cost});
                        AdjacencyList[to].push_back({from, latency, cost});
                        cout << "edge added\n";
                    }
                }
                break;
            }

            case 5: {
                cout << "how many nodes: "; int k; cin >> k;
                cin.ignore(); 
                for (int i = 0; i < k; i++) {
                    cout << "id: ";       long long id; cin >> id;
                    cin.ignore();
                    cout << "location: "; 
                    string location;
                    getline(cin, location); 
                    if (validNode(id, nodeName)) {
                        cout << "node [" << id << "] " << nodeName[id] << " already exists\n";
                    }
                    else {
                        nodeName[id] = location;
                        nodesSize++;
                        cout << "node added\n";
                    }
                }
                break;
            }

            case 6: {
                cout << "how many nodes to delete: ";
                int d; cin >> d;
                numDelete(d, AdjacencyList, nodeName);
                nodesSize = nodeName.size();
                break;
            }

            case 7: {
                cout << "how many edges to delete: ";
                int d; cin >> d;
                deleteEdges(d, AdjacencyList, nodeName);
                break;
            }

            case 8: {
                long long wt;
                int choice;
                cout << "1 for latency optimized, 2 for cost optimized: ";
                cin >> wt;
                cout << "1 to print total only, 2 to show full MST: ";
                cin >> choice;
                bool printEdges = (choice == 2);
                mst = Prim(AdjacencyList, wt, printEdges, nodeName,true);
                break;
            }

            case 9: {
                long long wt;
                int choice;
                cout << "1 for latency optimized, 2 for cost optimized: ";
                cin >> wt;
                cout << "1 to print total only, 2 to show full MST: ";
                cin >> choice;
                bool printEdges = (choice == 2);
                mst = Kruskal(AdjacencyList, wt, printEdges, nodeName,true);
                break;
            }

            case 10: {
                if (AdjacencyList.empty()) {
                    cout << "graph is empty\n";
                    break;
                }
                long long wt;
                cout << "1 for latency comparison, 2 for cost comparison: ";
                cin >> wt;

                cout << "\nrunning Prim...\n";
                vector<tuple<long long, long long, long long>> primMST = Prim(AdjacencyList, wt, false, nodeName,true);

                cout << "\nrunning Kruskal...\n";
                vector<tuple<long long, long long, long long>> kruskalMST = Kruskal(AdjacencyList, wt, false, nodeName,true);

                if (primMST.empty() || kruskalMST.empty()) {
                    cout << "one or both MSTs are empty, comparison not possible\n";
                    break;
                }
                long long primCost    = calculateMSTCost(primMST);
                long long kruskalCost = calculateMSTCost(kruskalMST);

                cout << "\n--- comparison result ---\n";
                cout << "Prim    total: " << primCost    << (wt == 1 ? " ms\n" : " USD\n");
                cout << "Kruskal total: " << kruskalCost << (wt == 1 ? " ms\n" : " USD\n");

                if (primCost == kruskalCost)
                    cout << "both give same optimal result\n";
                else
                    cout << "results differ (check graph or implementation)\n";

                cout << "Prim is better for dense graphs\n";
                cout << "Kruskal is better for sparse graphs\n";
                break;
            }

            case 11: {
                long long start, end;
                int filter;

                cout << "filter (1=latency, 2=cost): ";
                cin >> filter;

                cout << "source id: "; cin >> start;
                cout << "destination id: "; cin >> end;

                if (!validNode(start, nodeName) || !validNode(end, nodeName)) {
                    cout << "invalid nodes\n";
                    break;
                }

                if (start == end) {
                    cout << "source and destination are the same\n";
                    break;
                }

                vector<tuple<long long,long long,long long>> tempMST;

                tempMST = Prim(AdjacencyList, filter, false, nodeName, false);
                
                auto mstAdj = AdjacencyListForMst(tempMST);

                cout << "\n--- Dijkstra path (full graph) ---";
                shortestPathForOne(start, end, AdjacencyList, filter, nodeName);

                cout << "\n--- MST path (backbone only) ---";
                shortestPathForOneForMst(start, end, mstAdj, filter, nodeName);
                break;
            }

            case 12: {
                int filter;

                cout << "filter (1=latency, 2=cost): ";
                cin >> filter;

                vector<tuple<long long,long long,long long>> tempMST;

                
                tempMST = Prim(AdjacencyList, filter, false, nodeName, false);
                auto mstAdj = AdjacencyListForMst(tempMST);

                cout << "\n--- MST structure ---\n";
                for (auto &node : mstAdj) {
                    cout << "[" << node.first << "] " << nodeName[node.first] << " :\n";
                    for (auto &edge : node.second) {
                        cout << "   -> [" << get<0>(edge) << "] "
                            << nodeName[get<0>(edge)]
                            << "  weight: " << get<1>(edge)
                            << (filter == 1 ? " ms" : " USD") << "\n";
                    }
                }
                break;
            }

            case 13: {
                cout << "bye\n";
                break;
            }

            default:
                cout << "invalid choice\n";
        }
    }

    return 0;
}
