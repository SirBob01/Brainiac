#include "phenome.h"

namespace chess::neural {
    Phenome::Phenome(Genome &genome, std::vector<Point3> &inputs, std::vector<Point3> &outputs, PhenomeParameters params) 
            : _genome(genome), _inputs(inputs), _outputs(outputs) {
        _params = params;
        _node_count = 0;

        _hidden_activation = activations.at(_params.hidden_activation);
        _output_activation = activations.at(_params.output_activation);

        for(auto &input : _inputs) {
            _pointset[input] = _node_count;
            _nodes[_node_count] = {_genome.forward(input, {0, 0, 0}), NodeType::Input};
            _node_count++;
        }
        for(auto &output : _outputs) {
            _pointset[output] = _node_count;
            _nodes[_node_count] = {_genome.forward(output, {0, 0, 0}), NodeType::Output};
            _node_count++;
        }

        std::unordered_map<Edge, double, EdgeHash> conn1, conn2, conn3;
        std::unordered_set<int> hidden_nodes;
        
        // Input to hidden
        for(auto &input : _inputs) {
            Octree *root = division_initialization(input, true);
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
                Point3 hidden;
                for(auto &p : _pointset) {
                    if(_pointset[p.first] == hidden_id) {
                        hidden = p.first;
                        break;
                    }
                }
                Octree *root = division_initialization(hidden, true);
                prune_extract(hidden, root, true, conn2);
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
            Octree *root = division_initialization(output, false);
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
        
        _hidden_activation = rhs._hidden_activation;
        _output_activation = rhs._output_activation;
        return *this;
    }

    Octree *Phenome::division_initialization(Point3 point, bool outgoing) {
        Octree *root = new Octree(point, 1, 1);
        std::queue<Octree *> q;
        q.push(root);

        while(q.size()) {
            Octree *current = q.front();
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
              (current->level < _params.maximum_depth && current->get_variance() > _params.variance_threshold)) {
                for(auto &child : current->children) {
                    q.push(child);
                }
            }
        } 
        return root;
    }

    void Phenome::prune_extract(Point3 point, Octree *octree, bool outgoing,
                                std::unordered_map<Edge, double, EdgeHash> &connections) {
        if(_pointset.count(point) == 0) {
            _pointset[point] = _node_count;
            _nodes[_node_count] = {_genome.forward(point, {0, 0, 0}), NodeType::Hidden};
            _node_count++;
        }

        double d_top, d_bottom, d_left, d_right, d_front, d_back;
        for(auto &child : octree->children) {
            Point3 &center = child->center;
            if(child->get_variance() >= _params.variance_threshold) {
                prune_extract(point, child, outgoing, connections);
            }
            else {
                if(outgoing) {
                    d_left   = std::fabs(child->weight - _genome.forward(point, {center.x - child->size/2, center.y, center.z}));
                    d_right  = std::fabs(child->weight - _genome.forward(point, {center.x + child->size/2, center.y, center.z}));
                    d_top    = std::fabs(child->weight - _genome.forward(point, {center.x, center.y - child->size/2, center.z}));
                    d_bottom = std::fabs(child->weight - _genome.forward(point, {center.x, center.y + child->size/2, center.z}));
                    d_front  = std::fabs(child->weight - _genome.forward(point, {center.x, center.y, center.z + child->size/2}));
                    d_back   = std::fabs(child->weight - _genome.forward(point, {center.x, center.y, center.z - child->size/2}));
                }
                else {
                    d_left   = std::fabs(child->weight - _genome.forward({center.x - child->size/2, center.y, center.z}, point));
                    d_right  = std::fabs(child->weight - _genome.forward({center.x + child->size/2, center.y, center.z}, point));
                    d_top    = std::fabs(child->weight - _genome.forward({center.x, center.y - child->size/2, center.z}, point));
                    d_bottom = std::fabs(child->weight - _genome.forward({center.x, center.y + child->size/2, center.z}, point));
                    d_front  = std::fabs(child->weight - _genome.forward({center.x, center.y, center.z + child->size/2}, point));
                    d_back   = std::fabs(child->weight - _genome.forward({center.x, center.y, center.z - child->size/2}, point));
                }
                double band = std::max({
                    std::min(d_top, d_bottom), 
                    std::min(d_left, d_right), 
                    std::min(d_front, d_back)
                });
                if(band > _params.band_threshold) {
                    if(_pointset.count(center) == 0) {
                        _pointset[center] = _node_count;
                        _nodes[_node_count] = {_genome.forward(center, {0, 0, 0}), NodeType::Hidden};
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
        // Get collection of valid nodes
        std::unordered_set<int> valid_nodes;
        for(int n = 0; n < _node_count; n++) {
            bool input_path = false;
            for(auto &input : _inputs) {
                int node_id = _pointset[input];
                if(path_exists(n, node_id)) {
                    input_path = true;
                    break;
                }
            }
            bool output_path = false;
            for(auto &output : _outputs) {
                int node_id = _pointset[output];
                if(path_exists(node_id, n)) {
                    output_path = true;
                    break;
                }
            }
            if(input_path && output_path) {
                valid_nodes.insert(n);
            }
        }

        // Cull invalid nodes and connections
        for(int n = 0; n < _node_count; n++) {
            if(valid_nodes.count(n)) {
                continue;
            }
            std::vector<Edge> invalid_edges;
            for(auto &e : _edges) {
                if(n == e.first.from || n == e.first.to) {
                    invalid_edges.push_back(e.first);
                }
            }
            for(auto &edge : invalid_edges) {
                _edges.erase(edge);
            }
            _nodes.erase(n);
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
        generate_adjacency();
    }
    
    bool Phenome::active_output() {
        bool active = false;
        for(auto &output : _outputs) {
            active = active && _nodes[_pointset[output]].active;
        }
        return active;
    }

    std::vector<double> Phenome::forward(std::vector<double> input) {
        assert(input.size() == _inputs.size());
        int idx = 0;
        for(auto &point : _inputs) {
            NodePhenotype &node = _nodes[_pointset[point]];
            node.sum = input[idx++];
            node.activation = input[idx++];
            node.active = true;
        }

        // Forward propagation (adapted from the original NEAT implementation)
        int abort_count = 0;
        while(!active_output()) {
            if(abort_count++ >= 20) {
                break;
            }
            for(int i = 0; i < _node_count; i++) {
                NodePhenotype &node = _nodes[i];
                if(node.type != NodeType::Input) {
                    node.sum = 0;
                    for(auto &j : _adjacency[i]) {
                        NodePhenotype &incoming = _nodes[j];
                        if(incoming.active) {
                            node.sum += _edges[{j, i}] * incoming.activation;
                            node.active = true;
                        }
                    }
                }
            }
            for(int i = 0; i < _node_count; i++) {
                NodePhenotype &node = _nodes[i];
                if(node.type != NodeType::Input && node.active) {
                    switch(node.type) {
                        case NodeType::Hidden:
                            node.activation = _hidden_activation(node.sum);
                            break;
                        case NodeType::Output:
                            node.activation = _output_activation(node.sum);
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        // Calculate the final outputs
        std::vector<double> outputs;
        for(auto &point : _outputs) {
            outputs.push_back(_nodes[_pointset[point]].activation);
        }

        // Deactivate the nodes
        for(int i = 0; i < _node_count; i++) {
            _nodes[i].sum = 0;
            _nodes[i].activation = 0;
            _nodes[i].active = false;
        }

        return outputs;
    }

    double Phenome::get_fitness() {
        return _genome.get_fitness();
    }

    void Phenome::set_fitness(double fitness) {
        _genome.set_fitness(fitness);
    } 
}