#ifndef CHESS_NEURAL_GENOME_H_
#define CHESS_NEURAL_GENOME_H_

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cassert>

#include "activations.h"
#include "hyperparams.h"
#include "octree.h"
#include "random.h"

namespace chess::neural {
    enum NodeType {
        Input,
        Hidden,
        Output
    };

    struct Edge {
        int from;
        int to;

        bool operator==(const Edge &other) const;
    };

    struct EdgeHash {
        std::size_t operator()(Edge const &s) const noexcept;
    };

    struct EdgeGene {
        double weight;
        bool enabled;
    };

    struct NodeGene {
        double bias;
        NodeType type;
        activation_t function;

        double activation = 0;
        double sum = 0;
        bool active = false;
    };

    class Genome {
        std::unordered_map<Edge, EdgeGene, EdgeHash> _edges;
        std::vector<NodeGene> _nodes;

        std::vector<std::vector<int>> _adjacency;

        GenomeParameters _params;

        int _inputs;
        int _outputs;
        
        double _fitness;
        
        /**
         * Update the internal graph structure of the neural network for evaluation
         */
        void update_structure();

        /**
         * Add a new node between an existing edge and disable that edge
         */
        bool add_node(Edge edge);

        /**
         * Add a new edge between two unconnected nodes, and update 
         * the global innovation database
         */
        bool add_edge(Edge edge);

        /**
         * Enable an edge
         */
        bool enable_edge(Edge edge);

        /**
         * Disable an edge
         */
        bool disable_edge(Edge edge);

        /**
         * Shift the weight of an edge
         */
        void shift_weight(Edge edge);

        /**
         * Set the weight of an edge to a new random value
         */
        void reset_weight(Edge edge);
        
        /**
         * Shift the bias of a node
         */
        void shift_bias(int node);

        /**
         * Set the bias of a node to a new random value
         */
        void reset_bias(int node);

        /**
         * Change the activation function of a node
         */
        void change_activation(int node);

        /**
         * Test if the node id is an input
         */
        inline bool is_input(int node) {
            return node < _inputs;
        }

        /**
         * Test if the node id is an output
         */
        inline bool is_output(int node) {
            return node >= _inputs && node < _inputs + _outputs;
        }
        
        /**
         * Check if all the output nodes are active
         */
        bool active_output();

    public:
        /**
         * A genome represents the Compositional Pattern-Producing Network (CPPN)
         * used to generate the phenotype Artificial Neural Network (ANN).
         * 
         * The ANN is an 3-dimensional substrate, whose points correspond
         * to neurons. The weight between two neurons are determined by feeding their
         * coordinates to the evolved CPPN.
         * 
         * To calculate the bias at a point, feed only one point to the CPPN and
         * zero-out the rest, e.g., f(x1, y1, z1, 0, 0, 0).
         */
        Genome(GenomeParameters params);
        Genome(std::vector<NodeGene> &nodes, 
               std::unordered_map<Edge, EdgeGene, EdgeHash> &edges, 
               GenomeParameters params);

        /**
         * Feed-forward algorithm
         */
        double forward(Point3 p0, Point3 p1);

        /**
         * Randomly mutate the genome
         */
        void mutate();

        /**
         * Get the nodes of this network
         */
        const std::vector<NodeGene> &get_nodes();

        /**
         * Get the edges of this network
         */
        const std::unordered_map<Edge, EdgeGene, EdgeHash> &get_edges();

        /**
         * Get the fitness score of this genome
         */
        double get_fitness();

        /**
         * Set the fitness score of this genome
         */
        void set_fitness(double fitness);
    };
}

#endif