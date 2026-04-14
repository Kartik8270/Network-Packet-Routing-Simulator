# Network Packet Routing Simulator

A web-based visual application for network packet routing algorithms implemented in C++ with a Node.js backend and HTML/JavaScript frontend.

## Features

- **Shortest Path Algorithm**: Find shortest path between two nodes using Dijkstra's algorithm
- **Dijkstra All**: Calculate shortest paths from source to all nodes
- **Prim's MST**: Compute Minimum Spanning Tree using Prim's algorithm
- **Kruskal's MST**: Compute Minimum Spanning Tree using Kruskal's algorithm
- **Interactive Graph Visualization**: Visualize network topology with D3.js
- **Web Interface**: Modern dark-themed UI with three-panel layout

## Architecture

```
Frontend (HTML + JS + CSS)
↓
Node.js Backend (Express)
↓
C++ Executable (main.exe)
```

## Prerequisites

- C++ compiler (g++ or similar)
- Node.js (>= 14.0.0)
- npm

## Setup Instructions

### 1. Compile the C++ Code

```bash
# Compile main.cpp and all dependencies
g++ -std=c++17 main.cpp shortest_path.cpp mst.cpp utils.cpp -o main.exe
```

### 2. Install Node.js Dependencies

```bash
npm install
```

### 3. Start the Backend Server

```bash
npm start
```

The server will start on `http://localhost:3000`

### 4. Access the Web Application

Open your browser and navigate to `http://localhost:3000`

## Usage

### Web Interface

1. **Load Graph Data**: Click "Load Graph Data" to visualize the network
2. **Shortest Path**: Enter source and destination node IDs, select filter (latency/cost)
3. **Dijkstra All**: Enter source node ID and filter
4. **MST Algorithms**: Select filter and choose Prim's or Kruskal's algorithm

### Command Line Interface (Original CLI)

The CLI mode is preserved and can be used without arguments:

```bash
./main.exe
```

### Command Line Arguments (for backend)

```bash
# Shortest path
./main.exe shortest <from> <to> <filter>

# Dijkstra all nodes
./main.exe dijkstra_all <src> <filter>

# Prim's MST
./main.exe mst_prim <filter>

# Kruskal's MST
./main.exe mst_kruskal <filter>

# Filter values:
# 1 = latency (ms)
# 2 = cost (USD)
```

## API Endpoints

- `POST /shortest-path` - Calculate shortest path between two nodes
- `POST /dijkstra-all` - Calculate shortest paths from source to all nodes
- `POST /mst/prim` - Compute MST using Prim's algorithm
- `POST /mst/kruskal` - Compute MST using Kruskal's algorithm

## File Structure

```
├── main.cpp              # Main C++ file with CLI and command-line support
├── shortest_path.cpp     # Dijkstra's algorithm implementation
├── mst.cpp               # Prim's and Kruskal's MST algorithms
├── utils.cpp              # Utility functions
├── Data.json             # Network graph data
├── backend.js            # Node.js Express server
├── package.json          # Node.js dependencies
├── frontend/
│   ├── index.html        # Main HTML page
│   ├── style.css         # CSS styling
│   └── script.js         # JavaScript functionality
└── README.md             # This file
```

## Data Format

The `Data.json` file should contain:

```json
{
  "nodes": [
    {"id": 0, "location": "Node Name"},
    {"id": 1, "location": "Another Node"}
  ],
  "links": [
    {"source": 0, "target": 1, "latency_ms": 10, "cost_usd": 5}
  ]
}
```

## Development

### Running in Development Mode

```bash
npm run dev
```

This uses nodemon to automatically restart the server when files change.

### Building the C++ Executable

```bash
# For development
g++ -std=c++17 -g main.cpp shortest_path.cpp mst.cpp utils.cpp -o main.exe

# For production
g++ -std=c++17 -O2 main.cpp shortest_path.cpp mst.cpp utils.cpp -o main.exe
```

## Important Notes

- The CLI functionality is completely preserved
- All algorithms remain in C++ - no logic was moved to JavaScript
- The web interface provides the same functionality as the CLI
- CORS is enabled for cross-origin requests
- The graph visualization uses D3.js for interactive network display

## Troubleshooting

1. **Compilation errors**: Ensure all header files are present and C++17 is supported
2. **Server not starting**: Check if port 3000 is available
3. **API errors**: Ensure `main.exe` exists in the same directory as `backend.js`
4. **Graph not loading**: Verify `Data.json` exists and is properly formatted

## License

MIT License
