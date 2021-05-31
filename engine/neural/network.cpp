#include "network.h"

namespace chess::neural {
    Network::Network(NetworkParameters params) {
        int n = params.layers.size();

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        
        for(int i = 0; i < n; i++) {
            _a.emplace_back(1, params.layers[i].nodes, 0.0);

            if(i < n - 1) {
                // Add weight matrix (i, i+1)
                _weights.emplace_back(params.layers[i].nodes, params.layers[i+1].nodes);
                for(int j = 0; j < params.layers[i].nodes; j++) {
                    for(int k = 0; k < params.layers[i+1].nodes; k++) {
                        double r = params.random_range * distribution(generator) - (params.random_range / 2);
                        _weights.back().set_at(j, k, r);
                    }
                }
            }
            if(i > 0) {
                _z.emplace_back(1, params.layers[i].nodes, 0.0);
                _biases.emplace_back(1, params.layers[i].nodes, 0.0);
                _activations.push_back(params.layers[i].activation_function);
            }
        }

        _cost = params.cost_function;
        _learning_rate = params.learning_rate;
        _gradient_clip = params.gradient_clip;
    }

    Matrix Network::apply_cost(Matrix &predicted, Matrix &expected, bool derivative) {
        int rows = expected.get_rows();
        int cols = expected.get_cols();
        Matrix loss(expected.get_rows(), expected.get_cols());
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                loss.set_at(i, j, _cost(predicted.get_at(i, j), expected.get_at(i, j), derivative));
            }
        }
        return loss;
    }

    Matrix Network::forward(Matrix input) {
        _a[0] = input;

        int n = _weights.size();
        for(int i = 0; i < n; i++) {
            _z[i] = (_a[i] * _weights[i]) + _biases[i];
            _a[i+1] = _z[i];
            _a[i+1].map([this, i](double &n) {
                return _activations[i](n, false);
            });
        }
        return _a.back();
    }

    void Network::backward(Matrix expected) {
        Matrix delta_l = apply_cost(_a.back(), expected, true);
        int n = _activations.size();
        for(int i = n - 1; i >= 0; i--) {
            if(i < n - 1) {
                delta_l *= _weights[i+1].transpose();
            }
            _z[i].map([this, i](double &n) {
                return _activations[i](n, true);
            });
            delta_l ^= _z[i];

            Matrix weight_grad = (_a[i].transpose() * delta_l) * _learning_rate;
            double weight_grad_norm = weight_grad.norm();
            if(weight_grad_norm >_gradient_clip) {
                weight_grad *= _gradient_clip / weight_grad_norm;
            }

            Matrix bias_grad = delta_l * _learning_rate;
            double bias_grad_norm = bias_grad.norm();
            if(bias_grad_norm >_gradient_clip) {
                bias_grad *= _gradient_clip / bias_grad_norm;
            }

            // Update weights and biases
            _weights[i] -= weight_grad;
            _biases[i] -= bias_grad;
        }
    }

    void Network::fit(std::vector<double> input, std::vector<double> output) {
        forward({1, _weights[0].get_rows(), input});
        backward({1, _z.back().get_cols(), output});
    }
}