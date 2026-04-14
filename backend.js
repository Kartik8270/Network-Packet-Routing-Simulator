const express = require('express');
const { exec } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 5002;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static('frontend'));

// Helper function to execute C++ program
function executeCpp(args) {
    return new Promise((resolve, reject) => {
        const command = `main.exe ${args}`;
        exec(command, { cwd: __dirname }, (error, stdout, stderr) => {
            if (error) {
                reject({ error: error.message, stderr });
            } else {
                resolve({ stdout, stderr });
            }
        });
    });
}

// API Endpoints

app.post('/shortest-path', async (req, res) => {
    try {
        const { from, to, filter } = req.body;
        
        if (from === undefined || to === undefined || filter === undefined) {
            return res.status(400).json({ error: 'Missing required parameters: from, to, filter' });
        }
        
        const result = await executeCpp(`shortest ${from} ${to} ${filter}`);
        res.json({ 
            success: true, 
            output: result.stdout,
            error: result.stderr 
        });
    } catch (error) {
        res.status(500).json({ 
            success: false, 
            error: error.error || 'Internal server error' 
        });
    }
});

app.post('/dijkstra-all', async (req, res) => {
    try {
        const { src, filter } = req.body;
        
        if (src === undefined || filter === undefined) {
            return res.status(400).json({ error: 'Missing required parameters: src, filter' });
        }
        
        const result = await executeCpp(`dijkstra_all ${src} ${filter}`);
        res.json({ 
            success: true, 
            output: result.stdout,
            error: result.stderr 
        });
    } catch (error) {
        res.status(500).json({ 
            success: false, 
            error: error.error || 'Internal server error' 
        });
    }
});

app.post('/mst/prim', async (req, res) => {
    try {
        const { filter } = req.body;
        
        if (!filter) {
            return res.status(400).json({ error: 'Missing required parameter: filter' });
        }
        
        const result = await executeCpp(`mst_prim ${filter}`);
        res.json({ 
            success: true, 
            output: result.stdout,
            error: result.stderr 
        });
    } catch (error) {
        res.status(500).json({ 
            success: false, 
            error: error.error || 'Internal server error' 
        });
    }
});

app.post('/mst/kruskal', async (req, res) => {
    try {
        const { filter } = req.body;
        
        if (filter === undefined) {
            return res.status(400).json({ error: 'Missing required parameter: filter' });
        }
        
        const result = await executeCpp(`mst_kruskal ${filter}`);
        res.json({ 
            success: true, 
            output: result.stdout,
            error: result.stderr 
        });
    } catch (error) {
        res.status(500).json({ 
            success: false, 
            error: error.error || 'Internal server error' 
        });
    }
});

// Serve the frontend
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'frontend', 'index.html'));
});

// Start server
app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
    console.log('Available endpoints:');
    console.log('  POST /shortest-path');
    console.log('  POST /dijkstra-all');
    console.log('  POST /mst/prim');
    console.log('  POST /mst/kruskal');
});
