#include "phenome.h"

namespace chess::neural {
    Phenome::Phenome(Genome &genome, std::vector<Point3> &inputs, std::vector<Point3> &outputs, PhenomeParameters params) 
            : _genome(genome), _inputs(inputs), _outputs(outputs) {
        _params = params;
        _node_count = 0;

        // TODO: Perform ES-HyperNEAT algorithm here
        // Input to hidden
        for(auto &input : _inputs) {
            Octree *root = division_initialization(input, true);
            prune_extract(input, root, true);
            delete root;
        }

        // Hidden to hidden
        for(int i = 0; i < _params.iteration_level; i++) {

        }

        // Hidden to output
        for(auto &output : _outputs) {
            Octree *root = division_initialization(output, true);
            prune_extract(output, root, true);
            delete root;
        }
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

    void Phenome::prune_extract(Point3 point, Octree *octree, bool outgoing) {
        if(_pointset.count(point) == 0) {
            _pointset[point] = _node_count;
            _nodes[_node_count] = {0, _genome.forward(point, {0, 0, 0})};
            _node_count++;
        }

        double d_top, d_bottom, d_left, d_right, d_front, d_back;
        for(auto &child : octree->children) {
            Point3 &center = child->center;
            if(child->get_variance() >= _params.variance_threshold) {
                prune_extract(point, child, outgoing);
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
                if(std::max({
                    std::min(d_top, d_bottom), 
                    std::min(d_left, d_right), 
                    std::min(d_front, d_back)
                }) > _params.band_threshold) {
                    if(_pointset.count(center) == 0) {
                        _pointset[center] = _node_count;
                        _nodes[_node_count] = {0, _genome.forward(center, {0, 0, 0})};
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
                    _edges[edge] = child->weight * _params.weight_range;
                }
            }
        }
    }
    
    std::vector<double> Phenome::forward(std::vector<double> input) {
        assert(input.size() == _inputs.size());
        // TODO: Evaluate the neural network
        return {1.0};
    }
}