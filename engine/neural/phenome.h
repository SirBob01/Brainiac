#ifndef CHESS_NEURAL_PHENOME_H_
#define CHESS_NEURAL_PHENOME_H_

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cassert>

#include "genome.h"
#include "quadtree.h"

namespace chess::neural {
    class Phenome {
        Genome &_genome;
        PhenomeParameters _params;
        int _node_count;
        
        std::vector<Point> &_inputs;
        std::vector<Point> &_outputs;

        // Map 3D substrate points to node indexes
        std::unordered_map<Point, int, PointHash> _pointset;

        std::unordered_map<int, NodeGene> _nodes;
        std::unordered_map<Edge, double, EdgeHash> _edges;

        std::unordered_map<int, std::vector<int>> _adjacency;

        /**
         * Query the CPPN (genome) to generate weight and bias values
         */
        double calculate_weight(Point p0, Point p1);

        /**
         * Perform the division and initialization step of the evolving substrate
         */
        Quadtree *division_initialization(Point point, bool outgoing);

        /**
         * Perform the prune and extract algorithm of the evolving substrate
         */
        void prune_extract(Point point, Quadtree *quadtree, bool outgoing,
                           std::unordered_map<Edge, double, EdgeHash> &connections);

        /**
         * Check if a path exists between two nodes using BFS
         */
        bool path_exists(int start, int end);

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
        Phenome(Genome &genome, std::vector<Point> &inputs, std::vector<Point> &outputs, PhenomeParameters params);
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

        /**
         * Get this phenome's genome
         */
        Genome &get_genome();
    };
}

#endif