#ifndef CHESS_NEURAL_SPECIES_H_
#define CHESS_NEURAL_SPECIES_H_

#include "genome.h"
#include "hyperparams.h"

namespace chess::neural {
    class Specie {
        std::vector<Genome> _members;
        NEATParameters _params;
        int _stagnation_count;

    public:
        // Network parameters are going to propagated to all genomes
        Specie(NEATParameters params);

        /**
         * Add a new genome to the specie
         */
        void add(Genome genome);

        /**
         * Get the size of the specie
         */
        int get_size();

        /**
         * Get the representative genome of the specie for distance matching
         */
        Genome get_repr();

        /**
         * Let two members perform genetic crossover, or mutate an existing node
         */
        void repopulate();
    };
}

#endif