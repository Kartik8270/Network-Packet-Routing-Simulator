class NetworkGraph {
    constructor() {
        this.graphData = null;
        this.simulation = null;
        this.svg = null;
        this.selectedNodes = new Set();
        this.idToName = new Map();
        this.nameToId = new Map();
        this.zoom = null;
        this.tooltip = null;
    }

    async loadGraphData() {
        try {
            const response = await fetch('/Data.json');
            if (!response.ok) {
                throw new Error('Failed to load graph data');
            }
            this.graphData = await response.json();
            
            // Create name-to-ID and ID-to-name mappings
            this.graphData.nodes.forEach(node => {
                this.idToName.set(node.id, node.location);
                this.nameToId.set(node.location, node.id);
            });
            
            console.log('Graph data loaded:', this.graphData);
            console.log('ID to Name mapping:', this.idToName);
            console.log('Name to ID mapping:', this.nameToId);
            
            this.renderGraph();
            return true;
        } catch (error) {
            console.error('Error loading graph data:', error);
            showOutput('Error loading graph data: ' + error.message, 'error');
            return false;
        }
    }

    renderGraph() {
        if (!this.graphData) return;

        const container = document.getElementById('graph-container');
        const width = container.clientWidth;
        const height = container.clientHeight;

        // Clear existing SVG
        d3.select('#graph-svg').selectAll('*').remove();

        // Create tooltip
        this.tooltip = d3.select('body').append('div')
            .attr('class', 'node-tooltip')
            .style('opacity', 0);

        this.svg = d3.select('#graph-svg')
            .attr('width', width)
            .attr('height', height);

        // Add zoom behavior
        this.zoom = d3.zoom()
            .scaleExtent([0.5, 3])
            .on('zoom', (event) => {
                this.svg.select('g').attr('transform', event.transform);
            });

        this.svg.call(this.zoom);

        // Create main group for zoom/pan
        const g = this.svg.append('g');

        // Create force simulation
        this.simulation = d3.forceSimulation(this.graphData.nodes)
            .force('link', d3.forceLink(this.graphData.links).id(d => d.id).distance(120))
            .force('charge', d3.forceManyBody().strength(-400))
            .force('center', d3.forceCenter(width / 2, height / 2))
            .force('collision', d3.forceCollide().radius(35));

        // Create links (undirected - no arrows)
        const links = g.append('g')
            .attr('class', 'links')
            .selectAll('line')
            .data(this.graphData.links)
            .enter().append('line')
            .attr('class', 'link');

        // Create link labels
        const linkLabels = g.append('g')
            .attr('class', 'link-labels')
            .selectAll('text')
            .data(this.graphData.links)
            .enter().append('text')
            .attr('class', 'link-label')
            .text(d => `${d.latency_ms}ms | $${d.cost_usd}`);

        // Create nodes with improved styling
        const nodes = g.append('g')
            .attr('class', 'nodes')
            .selectAll('circle')
            .data(this.graphData.nodes)
            .enter().append('circle')
            .attr('class', 'node')
            .attr('r', 16)
            .call(this.drag());

        // Create node ID labels (inside circles)
        const nodeLabels = g.append('g')
            .attr('class', 'node-labels')
            .selectAll('text')
            .data(this.graphData.nodes)
            .enter().append('text')
            .attr('class', 'node-label')
            .text(d => d.id);

        // Create node location labels (below circles)
        const locationLabels = g.append('g')
            .attr('class', 'node-location-labels')
            .selectAll('text')
            .data(this.graphData.nodes)
            .enter().append('text')
            .attr('class', 'node-location-label')
            .attr('dy', 25)
            .text(d => d.location);

        // Add hover handlers for nodes
        nodes
            .on('mouseenter', (event, d) => {
                this.showTooltip(event, d);
            })
            .on('mousemove', (event) => {
                this.moveTooltip(event);
            })
            .on('mouseleave', () => {
                this.hideTooltip();
            })
            .on('click', (event, d) => {
                if (this.selectedNodes.has(d.id)) {
                    this.selectedNodes.delete(d.id);
                    d3.select(event.target).classed('selected', false);
                } else {
                    this.selectedNodes.add(d.id);
                    d3.select(event.target).classed('selected', true);
                }
            });

        // Update positions on simulation tick
        this.simulation.on('tick', () => {
            links
                .attr('x1', d => d.source.x)
                .attr('y1', d => d.source.y)
                .attr('x2', d => d.target.x)
                .attr('y2', d => d.target.y);

            linkLabels
                .attr('x', d => (d.source.x + d.target.x) / 2)
                .attr('y', d => (d.source.y + d.target.y) / 2);

            nodes
                .attr('cx', d => d.x)
                .attr('cy', d => d.y);

            nodeLabels
                .attr('x', d => d.x)
                .attr('y', d => d.y);

            locationLabels
                .attr('x', d => d.x)
                .attr('y', d => d.y);
        });
    }

    showTooltip(event, node) {
        const neighbors = this.graphData.links.filter(l => 
            l.source.id === node.id || l.target.id === node.id
        ).length;

        const neighborNames = this.graphData.links
            .filter(l => l.source.id === node.id)
            .map(l => l.target.location)
            .concat(
                this.graphData.links
                    .filter(l => l.target.id === node.id)
                    .map(l => l.source.location)
            );

        this.tooltip.transition()
            .duration(200)
            .style('opacity', 1);

        this.tooltip.html(`
            <h4>Node ${node.id}</h4>
            <p>${node.location}</p>
            <div class="tooltip-stat">
                <span class="tooltip-label">Connections:</span>
                <span class="tooltip-value">${neighbors}</span>
            </div>
            <div class="tooltip-stat">
                <span class="tooltip-label">Neighbors:</span>
                <span class="tooltip-value">${neighborNames.join(', ')}</span>
            </div>
        `);
    }

    moveTooltip(event) {
        this.tooltip
            .style('left', (event.pageX + 10) + 'px')
            .style('top', (event.pageY - 10) + 'px');
    }

    hideTooltip() {
        this.tooltip.transition()
            .duration(200)
            .style('opacity', 0);
    }

    drag() {
        return d3.drag()
            .on('start', (event, d) => {
                if (!event.active) this.simulation.alphaTarget(0.3).restart();
                d.fx = d.x;
                d.fy = d.y;
            })
            .on('drag', (event, d) => {
                d.fx = event.x;
                d.fy = event.y;
            })
            .on('end', (event, d) => {
                if (!event.active) this.simulation.alphaTarget(0);
                d.fx = null;
                d.fy = null;
            });
    }

    highlightPath(pathNodes, type = 'default') {
        // Reset all links and nodes
        d3.selectAll('.link').classed('highlighted', false).classed('dijkstra-highlighted', false).classed('mst-highlighted', false);
        
        if (type === 'dijkstra') {
            // Highlight Dijkstra path in GREEN
            d3.selectAll('.link')
                .classed('dijkstra-highlighted', function(d) {
                    return pathNodes.some((node, i) => {
                        if (i === 0) return false;
                        const prevNode = pathNodes[i - 1];
                        return (d.source.id === prevNode && d.target.id === node) ||
                               (d.source.id === node && d.target.id === prevNode);
                    });
                });
        } else {
            // Default highlighting
            d3.selectAll('.link')
                .classed('highlighted', function(d) {
                    return pathNodes.some((node, i) => {
                        if (i === 0) return false;
                        const prevNode = pathNodes[i - 1];
                        return (d.source.id === prevNode && d.target.id === node) ||
                               (d.source.id === node && d.target.id === prevNode);
                    });
                });
        }
    }

    highlightDijkstraPath(output) {
    // Parse the "path: CityA -> CityB -> CityC" line
    const pathMatch = output.match(/path:\s*(.+)/);
    if (!pathMatch) {
        console.log("highlightDijkstraPath: no path line found in output");
        return;
    }

    // Split by " -> " to get individual city names
    const cityNames = pathMatch[1].split('->').map(s => s.trim());
    console.log("Parsed city names:", cityNames);

    // Convert each city name to its numeric node ID using nameToId map
    const nodeIds = cityNames.map(name => {
        const id = this.nameToId.get(name);
        if (id === undefined) console.warn("No ID found for city:", name);
        return id;
    }).filter(id => id !== undefined);

    console.log("Converted node IDs:", nodeIds);

    if (nodeIds.length < 2) {
        console.log("highlightDijkstraPath: not enough nodes to form a path");
        return;
    }

    // Build edge pairs from consecutive node IDs
    const edges = [];
    for (let i = 0; i < nodeIds.length - 1; i++) {
        edges.push([nodeIds[i], nodeIds[i + 1]]);
    }

    console.log("Dijkstra edges to highlight:", edges);
    this.highlightEdges(edges, 'dijkstra');
}

    highlightMSTEdges(output, resetFirst = true) {
        console.log("highlightMSTEdges called with output:", output);
        console.log("resetFirst:", resetFirst);
        
        const edges = [];
        const lines = output.split('\n').map(l => l.replace(/\r/g, '').trim());
        let currentSource = null;

        console.log("Processing lines:", lines);

        for (const line of lines) {
            if (!line) continue;

            // Prim format source line: "[0] Ashburn, USA :"
            const sourceMatch = line.match(/^\s*\[(\d+)\]\s*.+\s*:?\s*$/);
            if (sourceMatch) {
                currentSource = parseInt(sourceMatch[1]);
                console.log("Found source:", currentSource, "from line:", line);
                continue;
            }

            // Prim format neighbor line: "-> [10] New York, USA  weight: 5"
            const neighborMatch = line.match(/^\s*->\s*\[(\d+)\]/);
            if (neighborMatch && currentSource !== null) {
                const target = parseInt(neighborMatch[1]);
                if (currentSource < target) {
                    edges.push([currentSource, target]);
                    console.log("Added edge:", [currentSource, target], "from line:", line);
                }
                continue;
            }

            // Kruskal format: "CityA <--> CityB  weight: N"
            const kruskalMatch = line.match(/^(.+?)\s*<-->\s*(.+?)\s+weight:/);
            if (kruskalMatch) {
                const idA = this.nameToId.get(kruskalMatch[1].trim());
                const idB = this.nameToId.get(kruskalMatch[2].trim());
                if (idA !== undefined && idB !== undefined) {
                    edges.push([idA, idB]);
                    console.log("Added Kruskal edge:", [idA, idB], "from line:", line);
                }
                continue;
            }

            // Try more permissive patterns for debugging
            const altSourceMatch = line.match(/^\[(\d+)\]/);
            if (altSourceMatch && !sourceMatch) {
                console.log("Alternative source match:", altSourceMatch[1], "line:", line);
                currentSource = parseInt(altSourceMatch[1]);
                continue;
            }

            const altNeighborMatch = line.match(/\[(\d+)\]/);
            if (altNeighborMatch && !neighborMatch && !kruskalMatch && currentSource !== null) {
                const target = parseInt(altNeighborMatch[1]);
                if (currentSource < target) {
                    edges.push([currentSource, target]);
                    console.log("Added alt edge:", [currentSource, target], "from line:", line);
                }
                continue;
            }

            // Fallback: try to match any line with [id] pattern for debugging
            const anyIdMatch = line.match(/\[(\d+)\]/);
            if (anyIdMatch) {
                console.log("Found ID in line:", anyIdMatch[1], "line:", line);
            }
        }

        console.log("MST edges parsed:", edges.length, edges);
        console.log("Calling highlightEdges with type 'mst'");
        
        // Test: Force some MST edges if none found
        if (edges.length === 0) {
            console.log("No MST edges found, adding test edges for debugging");
            edges.push([0, 10], [10, 5]); // Test edges
        }
        
        this.highlightEdges(edges, 'mst', resetFirst);
    }

    parseMSTEdges(output) {
        const edges = [];
        const lines = output.split('\n').map(l => l.replace(/\r/g, '').trim());
        let currentSource = null;

        for (const line of lines) {
            if (!line) continue;

            const sourceMatch = line.match(/^\[(\d+)\].+:$/);
            if (sourceMatch) {
                currentSource = parseInt(sourceMatch[1]);
                continue;
            }

            const neighborMatch = line.match(/^->\s*\[(\d+)\]/);
            if (neighborMatch && currentSource !== null) {
                const target = parseInt(neighborMatch[1]);
                if (currentSource < target) {
                    edges.push([currentSource, target]);
                }
                continue;
            }

            const kruskalMatch = line.match(/^(.+?)\s*<-->\s*(.+?)\s+weight:/);
            if (kruskalMatch) {
                const idA = this.nameToId.get(kruskalMatch[1].trim());
                const idB = this.nameToId.get(kruskalMatch[2].trim());
                if (idA !== undefined && idB !== undefined) {
                    edges.push([idA, idB]);
                }
                continue;
            }
        }
        return edges;
    }

    highlightMST() {
        // Highlight all edges in BLUE for MST
        d3.selectAll('.link').classed('mst-highlighted', true);
    }

    // Common edge highlighting function
    highlightEdges(edgeList, type, resetFirst = true) {
        console.log("Applying highlight type=" + type + " edges=", edgeList);

        if (resetFirst) {
            // Clear both CSS classes AND inline styles
            d3.selectAll('.link')
                .attr('stroke', null)
                .attr('stroke-width', null)
                .classed('dijkstra-highlighted', false)
                .classed('mst-highlighted', false)
                .classed('mst-path-highlighted', false);
        }

        if (edgeList.length === 0) {
            console.warn("highlightEdges: empty edge list, nothing to highlight");
            return;
        }

        // Apply highlight to matching edges
        d3.selectAll('.link')
            .filter(function(d) {
                const s = (typeof d.source === "object") ? d.source.id : d.source;
                const t = (typeof d.target === "object") ? d.target.id : d.target;
                return edgeList.some(([u, v]) =>
                    (u === s && v === t) || (u === t && v === s)
                );
            })
            .classed(type === 'dijkstra' ? 'dijkstra-highlighted' : 'mst-highlighted', true);
    }

    resetHighlight() {
        d3.selectAll('.link')
            .attr('stroke', null)
            .attr('stroke-width', null)
            .classed('highlighted', false)
            .classed('dijkstra-highlighted', false)
            .classed('mst-highlighted', false)
            .classed('mst-path-highlighted', false);
    }

    highlightEdgesWithClass(edgeList, cssClass, resetFirst = false) {
        if (resetFirst) {
            d3.selectAll('.link')
                .attr('stroke', null)
                .attr('stroke-width', null)
                .classed('dijkstra-highlighted', false)
                .classed('mst-highlighted', false)
                .classed('mst-path-highlighted', false);
        }

        if (edgeList.length === 0) {
            console.warn('highlightEdgesWithClass: empty edge list for class', cssClass);
            return;
        }

        d3.selectAll('.link')
            .filter(function(d) {
                const s = (typeof d.source === 'object') ? d.source.id : d.source;
                const t = (typeof d.target === 'object') ? d.target.id : d.target;
                return edgeList.some(([u, v]) =>
                    (u === s && v === t) || (u === t && v === s)
                );
            })
            .classed(cssClass, true);
    }

    // Graph editing methods
    addNode(id, name) {
        if (!this.graphData) return;
        
        // Check if node already exists
        if (this.graphData.nodes.find(n => n.id === id)) {
            console.log('Node already exists:', id);
            return false;
        }
        
        // Add new node
        const newNode = { id, location: name };
        this.graphData.nodes.push(newNode);
        
        console.log('Added node:', { id, name });
        this.renderGraph();
        return true;
    }

    deleteNode(id) {
        if (!this.graphData) return;
        
        // Remove node
        this.graphData.nodes = this.graphData.nodes.filter(n => n.id !== id);
        
        // Remove connected edges
        this.graphData.links = this.graphData.links.filter(l => l.source.id !== id && l.target.id !== id);
        
        console.log('Deleted node:', id);
        this.renderGraph();
        return true;
    }

    addEdge(fromId, toId, latency, cost) {
        if (!this.graphData) return;
        
        // Check if edge already exists
        const edgeExists = this.graphData.links.find(l => 
            (l.source.id === fromId && l.target.id === toId) ||
            (l.source.id === toId && l.target.id === fromId)
        );
        
        if (edgeExists) {
            console.log('Edge already exists:', fromId, '->', toId);
            return false;
        }
        
        // Add new edge
        const newEdge = { source: fromId, target: toId, latency_ms: latency, cost_usd: cost };
        this.graphData.links.push(newEdge);
        
        console.log('Added edge:', { fromId, toId, latency, cost });
        this.renderGraph();
        return true;
    }

    deleteEdge(fromId, toId) {
        if (!this.graphData) return;
        
        // Remove edge
        this.graphData.links = this.graphData.links.filter(l => 
            !((l.source.id === fromId && l.target.id === toId) ||
              (l.source.id === toId && l.target.id === fromId))
        );
        
        console.log('Deleted edge:', fromId, '->', toId);
        this.renderGraph();
        return true;
    }
}

class APIClient {
    constructor() {
        this.baseURL = '';
    }

    async request(endpoint, data) {
        try {
            const response = await fetch(this.baseURL + endpoint, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data)
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.error('API request failed:', error);
            throw error;
        }
    }

    async shortestPath(from, to, filter) {
        return this.request('/shortest-path', { from, to, filter });
    }

    async dijkstraAll(src, filter) {
        return this.request('/dijkstra-all', { src, filter });
    }

    async mstPrim(filter) {
        return this.request('/mst/prim', { filter });
    }

    async mstKruskal(filter) {
        return this.request('/mst/kruskal', { filter });
    }
}

// Global instances
let networkGraph = new NetworkGraph();
let apiClient = new APIClient();

// UI Functions
function showOutput(message, type = 'success') {
    const outputContent = document.getElementById('output-content');
    outputContent.innerHTML = `<div class="${type}">${message}</div>`;
}

function showLoading() {
    const outputContent = document.getElementById('output-content');
    outputContent.innerHTML = '<div class="loading"></div> Processing...';
}

function showResult(data) {
    const outputContent = document.getElementById('output-content');
    if (data.success) {
        let output = data.output;
        if (data.error) {
            output += '\n\nWarnings:\n' + data.error;
        }
        outputContent.textContent = output;
    } else {
        outputContent.innerHTML = `<div class="error">Error: ${data.error}</div>`;
    }
}

// Algorithm functions
async function runShortestPath() {
    const from = document.getElementById('sp-from').value;
    const to = document.getElementById('sp-to').value;
    const filter = document.getElementById('sp-filter').value;

    if (!from || !to) {
        showOutput('Please enter both source and destination node IDs', 'error');
        return;
    }

    showLoading();
    try {
        const result = await apiClient.shortestPath(from, to, filter);
        showResult(result);
        
        // Highlight Dijkstra path
        if (result.success) {
            networkGraph.highlightDijkstraPath(result.output);
        }
    } catch (error) {
        showOutput('Failed to execute shortest path algorithm: ' + error.message, 'error');
    }
}

async function runDijkstraAll() {
    const src = parseInt(document.getElementById('dijkstra-src').value);
    const filter = document.getElementById('dijkstra-filter').value;

    if (isNaN(src)) {
        showOutput('Please enter source node ID', 'error');
        return;
    }

    showLoading();
    try {
        const result = await apiClient.dijkstraAll(src, filter);
        showResult(result);

        if (result.success) {
            // Run Dijkstra in JS to get the shortest-path TREE (parent pointers)
            // so we know which edges to highlight
            const weightKey = filter === '1' ? 'latency_ms' : 'cost_usd';
            const links = networkGraph.graphData.links;
            const nodes = networkGraph.graphData.nodes;

            // Build adjacency list from current graph data
            const adj = new Map();
            nodes.forEach(n => adj.set(n.id, []));
            links.forEach(l => {
                const s = typeof l.source === 'object' ? l.source.id : l.source;
                const t = typeof l.target === 'object' ? l.target.id : l.target;
                const w = l[weightKey];
                adj.get(s).push({ node: t, weight: w });
                adj.get(t).push({ node: s, weight: w });
            });

            // Dijkstra with priority queue (min-heap via sorted array)
            const dist = new Map();
            const parent = new Map();
            nodes.forEach(n => dist.set(n.id, Infinity));
            dist.set(src, 0);
            parent.set(src, -1);

            // Simple priority queue using array + sort (fine for 30 nodes)
            const pq = [{ node: src, dist: 0 }];

            while (pq.length > 0) {
                pq.sort((a, b) => a.dist - b.dist);
                const { node, dist: d } = pq.shift();
                if (d > dist.get(node)) continue;

                for (const { node: neighbor, weight } of (adj.get(node) || [])) {
                    const newDist = d + weight;
                    if (newDist < dist.get(neighbor)) {
                        dist.set(neighbor, newDist);
                        parent.set(neighbor, node);
                        pq.push({ node: neighbor, dist: newDist });
                    }
                }
            }

            // Collect all tree edges (parent -> child for every reached node)
            const treeEdges = [];
            parent.forEach((par, node) => {
                if (par !== -1 && par !== undefined) {
                    treeEdges.push([par, node]);
                }
            });

            console.log("Dijkstra All tree edges:", treeEdges.length, treeEdges);
            networkGraph.highlightEdges(treeEdges, 'dijkstra');
        }
    } catch (error) {
        showOutput('Failed to execute Dijkstra algorithm: ' + error.message, 'error');
    }
}

async function runMSTPrim() {
    const filter = document.getElementById('mst-filter').value;

    showLoading();
    try {
        const result = await apiClient.mstPrim(filter);
        showResult(result);
        
        // Highlight MST edges
        if (result.success) {
            networkGraph.highlightMSTEdges(result.output);
        }
    } catch (error) {
        showOutput('Failed to execute Prim\'s algorithm: ' + error.message, 'error');
    }
}

async function runMSTKruskal() {
    const filter = document.getElementById('mst-filter').value;

    showLoading();
    try {
        const result = await apiClient.mstKruskal(filter);
        showResult(result);
        
        // Highlight MST edges
        if (result.success) {
            networkGraph.highlightMSTEdges(result.output);
        }
    } catch (error) {
        showOutput('Failed to execute Kruskal\'s algorithm: ' + error.message, 'error');
    }
}

async function runComparison() {
    const from = parseInt(document.getElementById('compare-from').value);
    const to = parseInt(document.getElementById('compare-to').value);
    const filter = document.getElementById('compare-filter').value;

    if (isNaN(from) || isNaN(to)) {
        showOutput('Please enter both source and destination node IDs for comparison', 'error');
        return;
    }

    if (from === to) {
        showOutput('Source and destination nodes must be different', 'error');
        return;
    }

    showLoading();
    try {
        // Run Dijkstra on full graph (source -> dest)
        const dijkstraResult = await apiClient.shortestPath(from, to, filter);

        // Run Prim MST to get backbone structure
        const mstResult = await apiClient.mstPrim(filter);

        // Compute MST path in JS by running Dijkstra on MST edges only
        let mstPathEdges = [];
        let mstPathOutput = '';
        if (mstResult.success) {
            const mstEdges = networkGraph.parseMSTEdges(mstResult.output);
            console.log("MST edges for path computation:", mstEdges.length, mstEdges);

            // Build MST adjacency map
            const weightKey = filter === '1' ? 'latency_ms' : 'cost_usd';
            const mstAdj = new Map();
            networkGraph.graphData.nodes.forEach(n => mstAdj.set(n.id, []));

            // For each MST edge, find the weight from the original graph
            mstEdges.forEach(([u, v]) => {
                const link = networkGraph.graphData.links.find(l => {
                    const ls = typeof l.source === 'object' ? l.source.id : l.source;
                    const lt = typeof l.target === 'object' ? l.target.id : l.target;
                    return (ls === u && lt === v) || (ls === v && lt === u);
                });
                if (link) {
                    const w = link[weightKey];
                    if (mstAdj.has(u)) mstAdj.get(u).push({ node: v, weight: w });
                    if (mstAdj.has(v)) mstAdj.get(v).push({ node: u, weight: w });
                }
            });

            // Dijkstra on MST graph
            const dist = new Map();
            const parent = new Map();
            networkGraph.graphData.nodes.forEach(n => dist.set(n.id, Infinity));
            dist.set(from, 0);
            parent.set(from, -1);
            const pq = [{ node: from, dist: 0 }];

            while (pq.length > 0) {
                pq.sort((a, b) => a.dist - b.dist);
                const { node, dist: d } = pq.shift();
                if (d > dist.get(node)) continue;

                for (const { node: neighbor, weight } of (mstAdj.get(node) || [])) {
                    const newDist = d + weight;
                    if (newDist < dist.get(neighbor)) {
                        dist.set(neighbor, newDist);
                        parent.set(neighbor, node);
                        pq.push({ node: neighbor, dist: newDist });
                    }
                }
            }

            // Reconstruct path
            if (dist.get(to) !== Infinity) {
                const path = [];
                let cur = to;
                while (cur !== -1 && cur !== undefined) {
                    path.unshift(cur);
                    cur = parent.get(cur);
                }
                // Build edge pairs for path
                for (let i = 0; i < path.length - 1; i++) {
                    mstPathEdges.push([path[i], path[i + 1]]);
                }
                const pathNames = path.map(id => networkGraph.idToName.get(id)).join(' -> ');
                mstPathOutput = `MST path: ${pathNames}\nhops: ${path.length - 1}\ntotal ${filter === '1' ? 'latency' : 'cost'}: ${dist.get(to)} ${filter === '1' ? 'ms' : 'USD'}`;
            } else {
                mstPathOutput = 'No path found in MST backbone between these nodes.';
            }
        }

        // Display results
        displayComparisonResults(dijkstraResult, mstResult, filter, mstPathOutput);

        // Highlight all three: MST structure (blue) + Dijkstra path (green) + MST path (yellow)
        highlightComparisonResults(dijkstraResult, mstResult, mstPathEdges);

    } catch (error) {
        showOutput('Failed to run comparison: ' + error.message, 'error');
    }
}

function displayComparisonResults(dijkstraResult, mstResult, filter, mstPathOutput = '') {
    const outputContent = document.getElementById('output-content');

    let output = '<div class="comparison-results">';

    // DIJKSTRA RESULT SECTION
    output += '<div class="result-section">';
    output += '<h3 style="color: var(--accent-green)"> Dijkstra Path (Full Graph):</h3>';
    if (dijkstraResult.success) {
        output += '<div class="result-content">';
        output += '<pre>' + dijkstraResult.output + '</pre>';
        const pathMatch = dijkstraResult.output.match(/path: (.+)/);
        const hopsMatch = dijkstraResult.output.match(/hops: (\d+)/);
        const totalMatch = dijkstraResult.output.match(/total (?:latency|cost): \$?([\d]+)/);
        if (pathMatch) output += '<p><strong>Path:</strong> ' + pathMatch[1] + '</p>';
        if (hopsMatch) output += '<p><strong>Hops:</strong> ' + hopsMatch[1] + '</p>';
        if (totalMatch) output += '<p><strong>Total:</strong> ' + totalMatch[1] + ' ' + (filter === '1' ? 'ms' : 'USD') + '</p>';
        output += '</div>';
    } else {
        output += '<div class="error">Error: ' + dijkstraResult.error + '</div>';
    }
    output += '</div>';

    // MST PATH SECTION
    output += '<div class="result-section">';
    output += '<h3 style="color: #f7c948"> Dijkstra Path (MST Backbone Only):</h3>';
    output += '<div class="result-content">';
    output += '<pre>' + (mstPathOutput || 'Not computed') + '</pre>';
    output += '</div>';
    output += '</div>';

    // MST STRUCTURE SECTION
    output += '<div class="result-section">';
    output += '<h3 style="color: var(--accent-blue)"> MST Backbone Structure (Prim):</h3>';
    if (mstResult.success) {
        output += '<div class="result-content">';
        output += '<pre>' + mstResult.output + '</pre>';
        const mstTotalMatch = mstResult.output.match(/total MST (?:latency|cost): \$?([\d]+)/);
        if (mstTotalMatch) output += '<p><strong>Total MST Weight:</strong> ' + mstTotalMatch[1] + ' ' + (filter === '1' ? 'ms' : 'USD') + '</p>';
        output += '</div>';
    } else {
        output += '<div class="error">Error: ' + mstResult.error + '</div>';
    }
    output += '</div>';

    // COMPARISON SUMMARY
    output += '<div class="comparison-summary">';
    output += '<h3>COMPARISON SUMMARY:</h3>';
    output += '<p><strong>&#128994; Green (animated):</strong> Shortest path on full graph</p>';
    output += '<p><strong>&#128995; Yellow (animated):</strong> Shortest path through MST backbone only</p>';
    output += '<p><strong>&#128996; Blue (solid):</strong> Full MST backbone structure</p>';
    output += '<p><strong>Note:</strong> Edges in both paths appear as the last-applied color.</p>';
    output += '</div>';
    
    output += '</div>';
    
    outputContent.innerHTML = output;
}

function highlightComparisonResults(dijkstraResult, mstResult, mstPathEdges) {
    console.log("highlightComparisonResults called");
    console.log("mstResult.success:", mstResult.success);
    console.log("mstResult.output:", mstResult.output);
    
    networkGraph.resetHighlight();

    // Layer 1: MST backbone structure in BLUE (solid) — drawn first as base layer
    if (mstResult.success) {
        console.log("Calling highlightMSTEdges with resetFirst=false");
        networkGraph.highlightMSTEdges(mstResult.output, false);
    }

    // Layer 2: Dijkstra full-graph path in GREEN (animated) — drawn on top
    if (dijkstraResult.success) {
        networkGraph.highlightDijkstraPath(dijkstraResult.output);
    }

    // Layer 3: MST path in YELLOW (animated reverse) — drawn last on top
    // Edges that are in both the Dijkstra path AND the MST path will show yellow
    // (last class wins for the stroke color due to !important in CSS order)
    if (mstPathEdges && mstPathEdges.length > 0) {
        networkGraph.highlightEdgesWithClass(mstPathEdges, 'mst-path-highlighted', false);
    }
}

// Graph editing functions
async function addNode() {
    const id = document.getElementById('add-node-id').value;
    const name = document.getElementById('add-node-name').value;
    
    if (!id || !name) {
        showOutput('Please enter both node ID and name', 'error');
        return;
    }
    
    const success = networkGraph.addNode(parseInt(id), name);
    if (success) {
        showOutput(`Node ${id} (${name}) added successfully!`);
        document.getElementById('add-node-id').value = '';
        document.getElementById('add-node-name').value = '';
    } else {
        showOutput(`Failed to add node: Node ${id} already exists`, 'error');
    }
}

async function deleteNode() {
    const id = document.getElementById('delete-node-id').value;
    
    if (!id) {
        showOutput('Please enter node ID', 'error');
        return;
    }
    
    const success = networkGraph.deleteNode(parseInt(id));
    if (success) {
        showOutput(`Node ${id} deleted successfully!`);
        document.getElementById('delete-node-id').value = '';
    } else {
        showOutput(`Failed to delete node: Node ${id} not found`, 'error');
    }
}

async function addEdge() {
    const fromId = document.getElementById('add-edge-from').value;
    const toId = document.getElementById('add-edge-to').value;
    const latency = document.getElementById('add-edge-latency').value;
    const cost = document.getElementById('add-edge-cost').value;
    
    if (!fromId || !toId || !latency || !cost) {
        showOutput('Please fill all edge fields', 'error');
        return;
    }
    
    const success = networkGraph.addEdge(parseInt(fromId), parseInt(toId), parseInt(latency), parseInt(cost));
    if (success) {
        showOutput(`Edge ${fromId} -> ${toId} added successfully!`);
        document.getElementById('add-edge-from').value = '';
        document.getElementById('add-edge-to').value = '';
        document.getElementById('add-edge-latency').value = '';
        document.getElementById('add-edge-cost').value = '';
    } else {
        showOutput(`Failed to add edge: Edge ${fromId} -> ${toId} already exists`, 'error');
    }
}

async function deleteEdge() {
    const fromId = document.getElementById('delete-edge-from').value;
    const toId = document.getElementById('delete-edge-to').value;
    
    if (!fromId || !toId) {
        showOutput('Please enter both from and to node IDs', 'error');
        return;
    }
    
    const success = networkGraph.deleteEdge(parseInt(fromId), parseInt(toId));
    if (success) {
        showOutput(`Edge ${fromId} -> ${toId} deleted successfully!`);
        document.getElementById('delete-edge-from').value = '';
        document.getElementById('delete-edge-to').value = '';
    } else {
        showOutput(`Failed to delete edge: Edge ${fromId} -> ${toId} not found`, 'error');
    }
}

// Zoom control functions
function zoomIn() {
    if (networkGraph.zoom) {
        networkGraph.svg.transition().duration(300).call(
            networkGraph.zoom.scaleBy, 1.3
        );
    }
}

function zoomOut() {
    if (networkGraph.zoom) {
        networkGraph.svg.transition().duration(300).call(
            networkGraph.zoom.scaleBy, 0.7
        );
    }
}

function resetZoom() {
    if (networkGraph.zoom) {
        networkGraph.svg.transition().duration(300).call(
            networkGraph.zoom.transform, d3.zoomIdentity
        );
    }
}

async function loadGraph() {
    showLoading();
    const success = await networkGraph.loadGraphData();
    if (success) {
        showOutput('Graph data loaded successfully!');
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    // Auto-load graph on page load
    loadGraph();
});

// Handle window resize
window.addEventListener('resize', () => {
    if (networkGraph.graphData) {
        networkGraph.renderGraph();
    }
});
