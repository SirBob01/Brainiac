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
                // Add weight matrix (i, i+1) and its delta
                _weights.emplace_back(params.layers[i].nodes, params.layers[i+1].nodes);
                _weight_deltas.emplace_back(params.layers[i].nodes, params.layers[i+1].nodes);

                for(int j = 0; j < params.layers[i].nodes; j++) {
                    for(int k = 0; k < params.layers[i+1].nodes; k++) {
                        double r = params.random_range * distribution(generator) - (params.random_range / 2);
                        _weights.back().set_at(j, k, r);
                    }
                }
            }
            if(i > 0) {
                _z.emplace_back(1, params.layers[i].nodes, 0.0);
                _activations.push_back(params.layers[i].activation_function);
            
                // Add bias matrix and its delta
                _bias_deltas.emplace_back(1, params.layers[i].nodes, 0.0);
                _biases.emplace_back(1, params.layers[i].nodes, 0.0);
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

    Matrix Network::forward(std::vector<double> input) {
        _a[0] = {1, _weights[0].get_rows(), input};

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

    void Network::fit(std::vector<DataSample> samples, int m) {
        m = std::min(m, static_cast<int>(samples.size()));
        double delta_factor = _learning_rate / m;

        for(int i = 0; i < samples.size(); i++) {
            DataSample &sample = samples[i];
            forward(sample.input);
            
            Matrix expected = {1, _z.back().get_cols(), sample.output};
            Matrix delta_l = apply_cost(_a.back(), expected, true);
            
            // Backpropagation algorithm
            int n = _activations.size();
            for(int j = n - 1; j >= 0; j--) {
                if(j < n - 1) {
                    delta_l *= _weights[j+1].transpose();
                }
                _z[j].map([this, j](double &n) {
                    return _activations[j](n, true);
                });
                delta_l ^= _z[j];

                _weight_deltas[j] += _a[j].transpose() * delta_l;
                _bias_deltas[j] += delta_l;
            }

            // Update the network parameters in batches
            if(i % m == 0) {
                for(int j = 0; j < _activations.size(); j++) {
                    Matrix weight_grad = _weight_deltas[j] * delta_factor;
                    double weight_grad_norm = weight_grad.norm();
                    if(weight_grad_norm >_gradient_clip) {
                        weight_grad *= _gradient_clip / weight_grad_norm;
                    }

                    Matrix bias_grad = _bias_deltas[j] * delta_factor;
                    double bias_grad_norm = bias_grad.norm();
                    if(bias_grad_norm >_gradient_clip) {
                        bias_grad *= _gradient_clip / bias_grad_norm;
                    }

                    _weights[j] -= weight_grad;
                    _biases[j] -= bias_grad;
                    
                    // Reset the deltas
                    _weight_deltas[j].zero();
                    _bias_deltas[j].zero();
                }
            }
        }
    }
}