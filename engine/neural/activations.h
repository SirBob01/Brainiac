#ifndef CHESS_NEURAL_ACTIVATIONS_H_
#define CHESS_NEURAL_ACTIVATIONS_H_

#include <algorithm>
#include <cmath>

namespace chess::neural {
    inline double lrelu(double x, bool derivative) {
        double slope = 0.05;
        if(derivative) {
            if(x > 0) {
                return 1.0;
            }
            else {
                return slope;
            }
        }
        else {
            return std::max(x, slope*x);
        }
    }

    inline double sigmoid(double x, bool derivative) {
        double y = 1.0/(1.0+std::exp(x));
        if(!derivative) {
            return y;
        }
        else {
            return y * (1 - y);
        }
    }

    inline double tanh(double x, bool derivative) {
        double f = 2 * sigmoid(2 * x, false) - 1;
        if(!derivative) {
            return f;
        }
        else {
            return 1 - f*f;
        }
    }
}

#endif