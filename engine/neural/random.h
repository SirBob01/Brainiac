#ifndef CHESS_NEURAL_RANDOM_H_
#define CHESS_NEURAL_RANDOM_H_

#include <random>

namespace chess::neural {
    static std::default_random_engine rng;
    static std::uniform_real_distribution<double> uniform(0, 1);
    
    double random();

    int randrange(int start, int stop);
}

#endif