#ifndef CHESS_NEURAL_BRAIN_H_
#define CHESS_NEURAL_BRAIN_H_

#include <algorithm>
#include <vector>
#include <deque>
#include <fstream>
#include <cstring>
#include <cassert>

#include "species.h"
#include "activations.h"
#include "phenome.h"

namespace chess::neural {
    class Brain {
        std::vector<Specie *> _species;
        std::vector<Phenome *> _phenomes;

        std::vector<Point> _inputs;
        std::vector<Point> _outputs;
        NEATParameters _params;

        std::vector<Genome *> _elites;
        std::deque<Genome *> _hall_of_fame;
        Genome *_global_best;

        int _generations;

        /**
         * Calculate the distance between genomes for speciation
         */
        double distance(Genome &a, Genome &b);

        /**
         * Perform sexual crossover between two genomes
         */
        Genome *crossover(Genome &a, Genome &b);

        /**
         * Add a new genome to an existing species, or create a new
         * one if it is too genetically distinct
         */
        void add_genome(Genome *genome);

        /**
         * Generate all the phenomes
         */
        void generate_phenomes();

        /**
         * Eliminate the worst in the population
         */
        void cull();

        /**
         * Randomly breed new genomes via mutation or crossover
         */
        void repopulate();

        /**
         * Randomly select a specie whose likelihoods depend on adjusted fitness sum
         */
        Specie *sample_specie();

        /**
         * Read a genome from an input filestream
         */
        Genome *read_genome(std::ifstream &infile);

        /**
         * Write a genome to an output filestream
         */
        void write_genome(std::ofstream &outfile, Genome *genome);

    public:
        Brain(std::vector<Point> inputs, std::vector<Point> outputs, NEATParameters params);
        Brain(std::string filename, NEATParameters params);
        ~Brain();

        /**
         * Get the list of phenomes for training
         */
        std::vector<Phenome *> &get_phenomes();

        /**
         * Evolve to the next generation
         */
        void evolve();

        /**
         * Get the current generation number
         */
        int get_generations();

        /**
         * Get the global fittest phenome
         */
        Phenome get_fittest();

        /**
         * Get the latest fittest phenome of the current generation
         */
        Phenome get_current_fittest();

        /**
         * Get the phenomes of those in the hall of fame
         */
        std::vector<Phenome> get_hall_of_fame();

        /**
         * Get the elite phenomes
         */
        std::vector<Phenome> get_elites();

        /**
         * Save the current population to disk
         */
        void save(std::string filename);
    };
}

#endif