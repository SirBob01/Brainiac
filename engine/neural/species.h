#ifndef CHESS_NEURAL_SPECIES_H_
#define CHESS_NEURAL_SPECIES_H_

#include <algorithm>

#include "genome.h"
#include "hyperparams.h"

namespace chess::neural {
    class Specie {
        std::vector<Genome *> _members;
        NEATParameters _params;
        int _stagnation_count;

    public:
        // Network parameters are going to propagated to all genomes
        Specie(Genome *representative, NEATParameters params);

        /**
         * Add a new genome to the specie
         */
        void add(Genome *genome);

        /**
         * Get the representative genome of the specie for distance matching
         */
        Genome &get_repr();

        /**
         * Get the members of the species
         */
        std::vector<Genome *> &get_members();

        /**
         * Destroy the worst performing genomes in the group
         */
        void cull();

        /**
         * Adjust the fitness scores of each of its members
         */
        void adjust_fitness();
    };
}

#endif