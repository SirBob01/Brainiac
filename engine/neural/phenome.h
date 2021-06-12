#ifndef CHESS_NEURAL_PHENOME_H_
#define CHESS_NEURAL_PHENOME_H_

#include <queue>
#include <algorithm>
#include <cassert>

#include "genome.h"
#include "octree.h"

namespace chess::neural {
    struct NodePhenotype {
        double value;
        double bias;
    };

    class Phenome {
        Genome &_genome;
        PhenomeParameters _params;
        int _node_count;
        
        std::vector<Point3> &_inputs;
        std::vector<Point3> &_outputs;

        std::unordered_map<Point3, int, Point3Hash> _pointset;
        std::unordered_map<int, NodePhenotype> _nodes;
        std::unordered_map<Edge, double, EdgeHash> _edges;

        /**
         * Perform the division and initialization step of the evolving substrate
         */
        Octree *division_initialization(Point3 point, bool outgoing);

        /**
         * Perform the prune and extract algorithm of the evolving substrate
         */
        void prune_extract(Point3 point, Octree *octree, bool outgoing);

    public:
        /**
         * A phenome is defined by the genome CPPN
         */
        Phenome(Genome &genome, std::vector<Point3> &inputs, std::vector<Point3> &outputs, PhenomeParameters params);

        /**
         * Evaluate the neural network
         */
        std::vector<double> forward(std::vector<double> input);
    };
}

#endif