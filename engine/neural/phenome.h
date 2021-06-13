#ifndef CHESS_NEURAL_PHENOME_H_
#define CHESS_NEURAL_PHENOME_H_

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cassert>

#include "genome.h"
#include "octree.h"

namespace chess::neural {
    struct NodePhenotype {
        double bias;
        NodeType type;

        double sum = 0;
        double activation = 0;
        bool active = false;
    };

    class Phenome {
        Genome &_genome;
        PhenomeParameters _params;
        int _node_count;
        
        std::vector<Point3> &_inputs;
        std::vector<Point3> &_outputs;

        // Map 3D substrate points to node indexes
        std::unordered_map<Point3, int, Point3Hash> _pointset;

        std::unordered_map<int, NodePhenotype> _nodes;
        std::unordered_map<Edge, double, EdgeHash> _edges;

        std::unordered_map<int, std::vector<int>> _adjacency;

        activation_t _hidden_activation;
        activation_t _output_activation;

        /**
         * Perform the division and initialization step of the evolving substrate
         */
        Octree *division_initialization(Point3 point, bool outgoing);

        /**
         * Perform the prune and extract algorithm of the evolving substrate
         */
        void prune_extract(Point3 point, Octree *octree, bool outgoing,
                           std::unordered_map<Edge, double, EdgeHash> &connections);

        /**
         * Check if a path exists between two nodes using BFS
         */
        bool path_exists(int from, int to);

        /**
         * Remove nodes and connections that do not have a path to inputs or outputs
         */
        void cleanup();
        
        /**
         * Generate the adjacency list
         */
        void generate_adjacency();

        /**
         * Update the internal graph structure of the neural network for evaluation
         */
        void update_structure();

        /**
         * Check if all the output nodes are active
         */
        bool active_output();

    public:
        /**
         * A phenome is defined by the genome CPPN
         */
        Phenome(Genome &genome, std::vector<Point3> &inputs, std::vector<Point3> &outputs, PhenomeParameters params);
        Phenome &operator=(const Phenome &rhs);

        /**
         * Evaluate the neural network
         */
        std::vector<double> forward(std::vector<double> input);

        /**
         * Get the fitness of this phenome's genome
         */
        double get_fitness();

        /**
         * Set the fitness of this phenome's genome
         */
        void set_fitness(double fitness);
    };
}

#endif