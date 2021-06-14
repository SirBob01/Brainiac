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
        _inputs = 4;
        _outputs = 1;

        _params = params;
        _fitness = 0.0;

        // Initialize the CPPN with minimal topology
        // Weights are random and biases are zero-initialized
        for(int i = 0; i < _inputs; i++) {
            auto random_it = std::next(std::begin(activations), randrange(0, activations.size()));
            _nodes.push_back({0, NodeType::Input, random_it->second});
        }
        for(int i = 0; i < _outputs; i++) {
            _nodes.push_back({0, NodeType::Output, tanh});
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
        _inputs = 4;
        _outputs = 1;

        _params = params;
        _fitness = 0.0;

        _nodes = nodes;
        _edges = edges;

        // Generate network structure from provided nodes and edges
        update_structure();
    }

    void Genome::update_structure() {
        // Update the adjacency list
        _adjacency.clear();
        _adjacency.resize(_nodes.size());
        for(auto &p : _edges) {
            Edge e = p.first;
            if(p.second.enabled) {
                _adjacency[e.to].push_back(e.from);
            }
        }
    }

    void Genome::add_node(Edge edge) {
        int new_node = _nodes.size();
        _edges[edge].enabled = false;
        _edges[{edge.from, new_node}] = {1.0, true};
        _edges[{new_node, edge.to}] = {_edges[edge].weight, true};

        auto random_it = std::next(std::begin(activations), randrange(0, activations.size()));
        _nodes.push_back({0, NodeType::Hidden, random_it->second});
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

    bool Genome::active_output() {
        bool active = true;
        for(int i = _inputs; i < _inputs + _outputs; i++) {
            active = active && _nodes[i].active;
        }
        return active;
    }

    double Genome::forward(Point p0, Point p1) {
        // Map the 3D points to the input vector
        for(int i = 0; i < _inputs; i++) {
            _nodes[i].active = true;
        }
        _nodes[0].activation = p0.x;
        _nodes[1].activation = p0.y;
        _nodes[2].activation = p1.x;
        _nodes[3].activation = p1.y;

        // Forward propagation (adapted from the original NEAT implementation)
        int n = _nodes.size();
        int abort_count = 0;
        while(!active_output()) {
            if(abort_count++ >= 20) {
                break;
            }
            for(int i = 0; i < n; i++) {
                NodeGene &node = _nodes[i];
                if(node.type != NodeType::Input) {
                    node.sum = 0;
                    for(auto &j : _adjacency[i]) {
                        NodeGene &incoming = _nodes[j];
                        if(incoming.active) {
                            node.sum += _edges[{j, i}].weight * incoming.activation;
                            node.active = true;
                        }
                    }
                }
            }
            for(int i = 0; i < n; i++) {
                NodeGene &node = _nodes[i];
                if(node.type != NodeType::Input && node.active) {
                    node.activation = node.function(node.sum + node.bias);
                }
            }
        }
        double result = _nodes[_inputs].activation;
        
        // Deactivate the nodes
        for(int i = 0; i < n; i++) {
            NodeGene &node = _nodes[i];
            node.sum = 0;
            node.activation = 0;
            node.active = false;
        }
        return result;
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
                add_node(random_edge->first);
            }
            cum_prob += _params.m_node;
            
            if(r > cum_prob && r <= cum_prob + _params.m_edge) {
                // Generate random nodes (i, j) such that:
                // 1. i is not an output
                // 2. j is not an input
                // 3. i != j
                int n = _nodes.size();
                std::vector<int> incoming;
                std::vector<int> remaining;
                for(int k = 0; k < n; k++) {
                    if(!is_output(k)) {
                        incoming.push_back(k);
                    }
                }
                int i = incoming[randrange(0, incoming.size())];

                for(int k = 0; k < n; k++) {
                    if(!is_input(k) && k != i) {
                        remaining.push_back(k);
                    }
                }
                int j = remaining[randrange(0, remaining.size())];
                error = !add_edge({i, j});
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

            if(r > cum_prob && r <= cum_prob + _params.m_activation) {
                int random_node = randrange(_inputs + _outputs, _nodes.size());
                change_activation(random_node);
            }
            cum_prob += _params.m_activation;
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