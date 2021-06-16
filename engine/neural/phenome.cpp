#include "phenome.h"

namespace chess::neural {
    Phenome::Phenome(Genome &genome, std::vector<Point> &inputs, std::vector<Point> &outputs, PhenomeParameters params) 
            : _genome(genome), _inputs(inputs), _outputs(outputs) {
        _params = params;
        _node_count = 0;

        for(auto &input : _inputs) {
            _pointset[input] = _node_count;
            _nodes[_node_count] = {_genome.forward(input, {0, 0}), NodeType::Input, nullptr};
            _node_count++;
        }
        for(auto &output : _outputs) {
            _pointset[output] = _node_count;
            _nodes[_node_count] = {_genome.forward(output, {0, 0}), NodeType::Output, _params.output_activation};
            _node_count++;
        }

        std::unordered_map<Edge, double, EdgeHash> conn1, conn2, conn3;
        std::unordered_set<int> hidden_nodes;
        
        // Input to hidden
        for(auto &input : _inputs) {
            Quadtree *root = division_initialization(input, true);
            prune_extract(input, root, true, conn1);
            delete root;
            for(auto &e : conn1) {
                hidden_nodes.insert(e.first.to);
            }
        }

        // Hidden to hidden
        std::unordered_set<int> unexplored_hidden = hidden_nodes;
        for(int i = 0; i < _params.iteration_level; i++) {
            for(auto &hidden_id : unexplored_hidden) {
                Point hidden;
                for(auto &p : _pointset) {
                    if(_pointset[p.first] == hidden_id) {
                        hidden = p.first;
                        break;
                    }
                }
                Quadtree *root = division_initialization(hidden, true);
                prune_extract(hidden, root, true, conn2);
                delete root;
                for(auto &e : conn2) {
                    hidden_nodes.insert(e.first.to);
                }
            }

            // Remove the just explored nodes
            std::unordered_set<int> new_unexplored = hidden_nodes;
            for(auto &u : unexplored_hidden) {
                new_unexplored.erase(u);
            }
            unexplored_hidden = new_unexplored;
        }

        // Hidden to output
        for(auto &output : _outputs) {
            Quadtree *root = division_initialization(output, false);
            prune_extract(output, root, false, conn3);
            delete root;
        }

        // Combine the connection collections
        for(auto &c : conn1) {
            _edges[c.first] = c.second;
        }
        for(auto &c : conn2) {
            _edges[c.first] = c.second;
        }
        for(auto &c : conn3) {
            _edges[c.first] = c.second;
        }
        update_structure();
    }

    Phenome &Phenome::operator=(const Phenome &rhs) {
        _genome = rhs._genome;
        _node_count = rhs._node_count;
        _params = rhs._params;
        
        _inputs = rhs._inputs;
        _outputs = rhs._outputs;
        
        _pointset = rhs._pointset;
        _nodes = rhs._nodes;
        _edges = rhs._edges;

        _adjacency = rhs._adjacency;
        return *this;
    }

    Quadtree *Phenome::division_initialization(Point point, bool outgoing) {
        Quadtree *root = new Quadtree(point, 1, 1);
        std::queue<Quadtree *> q;
        q.push(root);

        while(q.size()) {
            Quadtree *current = q.front();
            q.pop();
            current->generate_children();
            for(auto &child : current->children) {
                if(outgoing) {
                    child->weight = _genome.forward(point, child->center);
                }
                else {
                    child->weight = _genome.forward(child->center, point);
                }
            }

            if(current->level < _params.initial_depth || 
              (current->level < _params.maximum_depth && current->get_variance() > _params.division_threshold)) {
                for(auto &child : current->children) {
                    q.push(child);
                }
            }
        } 
        return root;
    }

    void Phenome::prune_extract(Point point, Quadtree *Quadtree, bool outgoing,
                                std::unordered_map<Edge, double, EdgeHash> &connections) {
        if(_pointset.count(point) == 0) {
            _pointset[point] = _node_count;
            _nodes[_node_count] = {_genome.forward(point, {0, 0}), NodeType::Hidden, _params.hidden_activation};
            _node_count++;
        }

        double d_top, d_bottom, d_left, d_right;
        for(auto &child : Quadtree->children) {
            Point &center = child->center;
            if(child->get_variance() >= _params.variance_threshold) {
                prune_extract(point, child, outgoing, connections);
            }
            else {
                if(outgoing) {
                    d_left   = std::fabs(child->weight - _genome.forward(point, {center.x - child->size/2, center.y}));
                    d_right  = std::fabs(child->weight - _genome.forward(point, {center.x + child->size/2, center.y}));
                    d_top    = std::fabs(child->weight - _genome.forward(point, {center.x, center.y - child->size/2}));
                    d_bottom = std::fabs(child->weight - _genome.forward(point, {center.x, center.y + child->size/2}));
                }
                else {
                    d_left   = std::fabs(child->weight - _genome.forward({center.x - child->size/2, center.y}, point));
                    d_right  = std::fabs(child->weight - _genome.forward({center.x + child->size/2, center.y}, point));
                    d_top    = std::fabs(child->weight - _genome.forward({center.x, center.y - child->size/2}, point));
                    d_bottom = std::fabs(child->weight - _genome.forward({center.x, center.y + child->size/2}, point));
                }
                double band = std::max(std::min(d_top, d_bottom), std::min(d_left, d_right));
                if(band > _params.band_threshold) {
                    if(_pointset.count(center) == 0) {
                        _pointset[center] = _node_count;
                        _nodes[_node_count] = {_genome.forward(center, {0, 0}), NodeType::Hidden, _params.hidden_activation};
                        _node_count++;
                    }

                    Edge edge;
                    if(outgoing) {
                        edge.from = _pointset[point];
                        edge.to = _pointset[center];
                    }
                    else {
                        edge.from = _pointset[center];
                        edge.to = _pointset[point];
                    }

                    // Assign edge to weight
                    connections[edge] = child->weight * _params.weight_range;
                }
            }
        }
    }

    bool Phenome::path_exists(int start, int end) {
        std::unordered_set<int> visited;
        std::queue<int> q;
        q.push(start);
        while(q.size()) {
            int current = q.front();
            q.pop();
            visited.insert(current);
            if(current == end) {
                return true;
            }
            for(auto &adj : _adjacency[current]) {
                if(!visited.count(adj)) {
                    q.push(adj);
                }
            }
        } 
        return false;
    }

    void Phenome::cleanup() {
        for(int n = 0; n < _node_count; n++) {
            // Assume input and output nodes are valid
            if(_nodes[n].type != NodeType::Hidden) {
                continue;
            }
            // Deal with hidden nodes
            bool input_path = false;
            for(auto &input : _inputs) {
                input_path = input_path || path_exists(n, _pointset[input]);
            }
            bool output_path = false;
            for(auto &output : _outputs) {
                output_path = output_path || path_exists(_pointset[output], n);
            }
            if(!input_path || !output_path) {
                _nodes.erase(n);
            }
        }

        // Cull invalid edges
        std::vector<Edge> invalid_edges;
        for(auto &e : _edges) {
            if(_nodes.count(e.first.from) == 0 || 
               _nodes.count(e.first.to) == 0) {
                invalid_edges.push_back(e.first);
            }
        }
        for(auto &edge : invalid_edges) {
            _edges.erase(edge);
        }
    }

    void Phenome::generate_adjacency() {
        // Generate the updated adjacency list
        _adjacency.clear();
        for(auto &p : _edges) {
            Edge e = p.first;
            _adjacency[e.to].push_back(e.from);
        }
    }

    void Phenome::update_structure() {
        generate_adjacency();
        cleanup();
        if(_edges.size() == 0) {
            // Algorithm was unable to generate hidden nodes 
            // connect input and output neurons directly    
            for(auto &input : _inputs) {
                for(auto &output : _outputs) {
                    int i = _pointset[input];
                    int j = _pointset[output];
                    _edges[{i, j}] = _genome.forward(input, output) * _params.weight_range;
                }
            }
        }
        generate_adjacency();
    }
    
    bool Phenome::active_output() {
        bool active = true;
        for(auto &output : _outputs) {
            active = active && _nodes[_pointset[output]].active;
        }
        return active;
    }

    std::vector<double> Phenome::forward(std::vector<double> input) {
        assert(input.size() == _inputs.size());
        int idx = 0;
        for(auto &point : _inputs) {
            NodeGene &node = _nodes[_pointset[point]];
            node.activation = input[idx++];
            node.active = true;
        }

        // Forward propagation (adapted from the original NEAT implementation)
        int abort_count = 0;
        while(!active_output()) {
            if(abort_count++ >= 20) {
                break;
            }
            for(auto &n : _nodes) {
                int i = n.first;
                NodeGene &node = n.second;
                if(node.type != NodeType::Input) {
                    node.sum = 0;
                    for(auto &j : _adjacency[i]) {
                        NodeGene &incoming = _nodes[j];
                        if(incoming.active) {
                            node.sum += _edges[{j, i}] * incoming.activation;
                            node.active = true;
                        }
                    }
                }
            }
            for(auto &n : _nodes) {
                auto &node = n.second;
                if(node.type != NodeType::Input && node.active) {
                    node.activation = node.function(node.sum + node.bias);
                }
            }
        }

        // Calculate the final outputs
        std::vector<double> outputs;
        for(auto &point : _outputs) {
            outputs.push_back(_nodes[_pointset[point]].activation);
        }

        // Deactivate the nodes
        for(auto &n : _nodes) {
            auto &node = n.second;
            node.sum = 0;
            node.activation = 0;
            node.active = false;
        }

        return outputs;
    }

    double Phenome::get_fitness() {
        return _genome.get_fitness();
    }

    void Phenome::set_fitness(double fitness) {
        _genome.set_fitness(fitness);
    } 

    Genome &Phenome::get_genome() {
        return _genome;
    }
}