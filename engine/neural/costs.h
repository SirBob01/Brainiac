#ifndef CHESS_NEURAL_COSTS_H_
#define CHESS_NEURAL_COSTS_H_

#include <algorithm>
#include <cmath>

namespace chess::neural {
    inline double quadratic_cost(double predicted_y, double expected_y, bool derivative) {
        double diff = expected_y - predicted_y;
        if(derivative) {
            return diff;
        }
        else {
            return 0.5 * diff * diff;
        }
    }
}

#endif