#include "genome.h"

namespace chess::neural {
    bool Edge::operator==(const Edge &other) const {
        return from == other.from && to == other.to;
    }

    std::size_t EdgeHash::operator()(Edge const &s) const noexcept {
        std::size_t h1 = std::hash<int>{}(s.from);
        std::size_t h2 = std::hash<int>{}(s.to);
        return h1 ^ (h2 << 1);
    }

    Genome::Genome(GenomeParameters params) {
        _inputs = 6;
        _outputs = 1;

        _params = params;
        _fitness = 0.0;

        // Initialize the CPPN with minimal topology
        // Weights are random and biases are zero-initialized
        for(int i = 0; i < _inputs + _outputs; i++) {
            auto random_it = std::next(std::begin(activations), randrange(0, activations.size()));
            _nodes.push_back({0, 0, random_it->second});
        }
        
        // Connect all inputs to all outputs
        for(int i = 0; i < _inputs; i++) {
            for(int j = _inputs; j < _inputs + _outputs; j++) {
                add_edge({i, j});
            }
        }

        // Generate the initial network structure
        update_structure();
    }

    Genome::Genome(std::vector<NodeGene> &nodes, 
                   std::unordered_map<Edge, EdgeGene, EdgeHash> &edges, 
                   GenomeParameters params) {
        _inputs = 6;
        _outputs = 1;

        _params = params;
        _fitness = 0.0;

        _nodes = nodes;
        _edges = edges;

        // Generate network structure from provided nodes and edges
        update_structure();
    }

    void Genome::topological_sort(int node, std::set<int> &visited) {
        if(visited.count(node)) {
            return;
        }
        
        for(int &adj : _adjacency[node]) {
            topological_sort(adj, visited);
        }

        visited.insert(node);
        _sorted.push_back(node);
    }

    void Genome::update_structure() {
        // Update the adjacency list
        _adjacency.clear();
        _adjacency.resize(_nodes.size());
        for(auto &p : _edges) {
            Edge e = p.first;
            _adjacency[e.to].push_back(e.from);
        }
        
        // Update the topological sort of the nodes
        int n = _nodes.size();
        _sorted.clear();
        std::set<int> visited;
        for(int i = 0; i < n; i++) {
            topological_sort(i, visited);
        }
    }

    bool Genome::add_node(Edge edge) {
        if(!disable_edge(edge)) {
            return false;
        }
        int new_node = _nodes.size();
        add_edge({edge.from, new_node});
        add_edge({new_node, edge.to});

        auto random_it = std::next(std::begin(activations), randrange(0, activations.size()));
        _nodes.push_back({0, 0, random_it->second});
        return true;
    }

    bool Genome::add_edge(Edge edge) {
        if(_edges.count(edge)) {
            return false;
        }
        _edges[edge] = {random() * 2.0 - 1.0, true};
        return true;
    }

    bool Genome::enable_edge(Edge edge) {
        if(_edges[edge].enabled) {
            return false;
        }
        _edges[edge].enabled = true;
        return true;
    }

    bool Genome::disable_edge(Edge edge) {
        if(!_edges[edge].enabled) {
            return false;
        }
        _edges[edge].enabled = false;
        return true;
    }
    
    void Genome::shift_weight(Edge edge) {
        _edges[edge].weight += random() * 2.0 - 1.0;
    }

    void Genome::reset_weight(Edge edge) {
        _edges[edge].weight = random() * 2.0 - 1.0;
    }

    void Genome::shift_bias(int node) {
        _nodes[node].bias += random() * 2.0 - 1.0;
    }

    void Genome::reset_bias(int node) {
        _nodes[node].bias = random() * 2.0 - 1.0;
    }

    void Genome::change_activation(int node) {
        auto random_it = std::next(std::begin(activations), randrange(0, activations.size()));
        _nodes[node].function = random_it->second;
    }

    double Genome::forward(Point3 p0, Point3 p1) {
        // Map the 3D points to the input vector
        _nodes[0].value = p0.x;
        _nodes[1].value = p0.y;
        _nodes[2].value = p0.z;
        _nodes[3].value = p1.x;
        _nodes[4].value = p1.y;
        _nodes[5].value = p1.z;

        // Forward propagation
        int n = _nodes.size();
        for(int i = _inputs; i < n; i++) {
            int to = _sorted[i];
            double sum = 0.0;
            for(int from : _adjacency[to]) {
                EdgeGene gene = _edges[{from, to}];
                if(gene.enabled) {
                    sum += gene.weight * _nodes[from].value + _nodes[to].bias;
                }
            }
            _nodes[to].value = _nodes[to].function(sum);
        }

        // Generate output between -1.0 and 1.0
        NodeGene &out_node = _nodes[_inputs];
        return tanh(out_node.value);
    }

    void Genome::mutate() {
        bool error;
        do {
            error = false;
            double r = random();
            
            double cum_prob = 0;
            if(r <= cum_prob + _params.m_weight_shift) {
                auto random_edge = std::next(std::begin(_edges), randrange(0, _edges.size()));
                shift_weight(random_edge->first);
            }
            cum_prob += _params.m_weight_shift;
            
            if(r > cum_prob && r <= cum_prob + _params.m_weight_change) {
                auto random_edge = std::next(std::begin(_edges), randrange(0, _edges.size()));
                reset_weight(random_edge->first);
            }
            cum_prob += _params.m_weight_change;

            if(r > cum_prob && r <= cum_prob + _params.m_bias_shift) {
                int random_node = randrange(_inputs, _nodes.size());
                shift_bias(random_node);
            }
            cum_prob += _params.m_bias_shift;

            if(r > cum_prob && r <= cum_prob + _params.m_bias_change) {
                int random_node = randrange(_inputs, _nodes.size());
                reset_bias(random_node);
            }
            cum_prob += _params.m_bias_change;

            if(r > cum_prob && r <= cum_prob + _params.m_node) {
                auto random_edge = std::next(std::begin(_edges), randrange(0, _edges.size()));
                error = !add_node(random_edge->first);
            }
            cum_prob += _params.m_node;
            
            if(r > cum_prob && r <= cum_prob + _params.m_edge) {
                // When adding a new edge, maintain the following invariants:
                // - Chosen nodes are ordered topologically (from -> to)
                // - The "from" node cannot be an output, and the "to" node cannot be an input
                int i = randrange(0, _nodes.size()-_outputs);
                int j = randrange(std::max(_inputs, i+1), _nodes.size());
                error = !add_edge({_sorted[i], _sorted[j]});
            }
            cum_prob += _params.m_edge;
            
            if(r > cum_prob && r <= cum_prob + _params.m_disable) {
                auto random_edge = std::next(std::begin(_edges), randrange(0, _edges.size()));
                error = !disable_edge(random_edge->first);
            }
            cum_prob += _params.m_disable;

            if(r > cum_prob && r <= cum_prob + _params.m_enable) {
                auto random_edge = std::next(std::begin(_edges), randrange(0, _edges.size()));
                error = !enable_edge(random_edge->first);
            }
            cum_prob += _params.m_enable;
        } while(error);

        update_structure();
    }

    const std::vector<NodeGene> &Genome::get_nodes() {
        return _nodes;
    }

    const std::unordered_map<Edge, EdgeGene, EdgeHash> &Genome::get_edges() {
        return _edges;
    }

    double Genome::get_fitness() {
        return _fitness;
    }

    void Genome::set_fitness(double fitness) {
        _fitness = fitness;
    }
}