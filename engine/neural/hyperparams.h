#ifndef CHESS_NEURAL_HYPERPARAMS_H_
#define CHESS_NEURAL_HYPERPARAMS_H_

#include "activations.h"

namespace chess::neural {
    struct GenomeParameters {
        double m_weight_shift  = 0.25;
        double m_weight_change = 0.05;
        double m_bias_shift    = 0.25;
        double m_bias_change   = 0.05;
        double m_node          = 0.05;
        double m_edge          = 0.05;
        double m_disable       = 0.05;
        double m_enable        = 0.15;
        double m_activation    = 0.1;
    };

    struct PhenomeParameters {
        double variance_threshold = 0.03;
        double band_threshold     = 0.3;
        double weight_range       = 1.0;
        int initial_depth         = 4;
        int maximum_depth         = 50;
        int iteration_level       = 1;

        // Activation functions of the ANN
        std::string hidden_activation = "lrelu";
        std::string output_activation = "tanh";
    };

    /** 
     * Hyperparameters get passed down to individual genomes and phenomes 
     */
    struct NEATParameters {
        int max_stagnation = 15;

        double crossover_probability = 0.3;
        double mutation_probability  = 0.7;

        double c1 = 1.0;
        double c2 = 0.4;

        double distance_threshold = 0.3;

        int population = 100;
        
        GenomeParameters genome_params;
        PhenomeParameters phenome_params;
    };
}

#endif