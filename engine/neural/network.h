#ifndef CHESS_NEURAL_NETWORK_H_
#define CHESS_NEURAL_NETWORK_H_

#include <vector>
#include <random>

#include "matrix.h"
#include "costs.h"

namespace chess::neural {
    // Activation/cost functions must take a double and a boolean indicating
    // whether or not it's the derivative function
    using activation = double (*)(double x, bool derivative);
    using cost = double (*)(double predicted_y, double expected_y, bool derivative);

    struct Layer {
        int nodes;
        activation activation_function;
    };

    struct NetworkParameters {
        cost cost_function   = quadratic_cost;
        double learning_rate = 1.0;
        double gradient_clip = 1.0;
        double random_range  = 1.0;
        std::vector<Layer> layers;
    };

    class Network {
        std::vector<Matrix> _z;
        std::vector<Matrix> _a;
        std::vector<Matrix> _biases;
        std::vector<Matrix> _weights;

        std::vector<activation> _activations;
        cost _cost;

        double _learning_rate;
        double _gradient_clip;

        /**
         * Apply the cost function to matrices
         */
        Matrix apply_cost(Matrix &predicted, Matrix &expected, bool derivative);

        /**
         * Feed-forward algorithm
         */
        Matrix forward(Matrix input);

        /**
         * Backpropagation gradient descent algorithm
         */
        void backward(Matrix expected);

    public:
        Network(NetworkParameters params);

        /**
         * Train network on some data
         */
        void fit(std::vector<double> input, std::vector<double> output);

        /**
         * Evaluate the network on some input data
         */
        inline Matrix evaluate(std::vector<double> input) {
            return forward({1, _weights[0].get_rows(), input});
        }
    };
}

#endif