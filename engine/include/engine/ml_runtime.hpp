#ifndef GAMEOFLIFES_ENGINE_ML_RUNTIME_HPP
#define GAMEOFLIFES_ENGINE_ML_RUNTIME_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace gol {

enum class Activation { None, Relu, Sigmoid };

struct Layer {
    // weights: rows x cols matrix stored row-major.
    // rows = output_size, cols = input_size.
    std::vector<float> weights;
    std::vector<float> biases;
    int input_size;
    int output_size;
    Activation activation;
};

struct FeedForwardNet {
    std::vector<Layer> layers;
    int input_size = 0;
    int output_size = 0;
};

// Load a model from JSON string. Throws std::runtime_error on invalid format.
FeedForwardNet load_model(const std::string& json_str);

// Run forward pass. Input size must match net.input_size.
std::vector<float> infer(const FeedForwardNet& net, const std::vector<float>& input);

// Return the index of the maximum value in the vector (argmax).
int argmax(const std::vector<float>& v);

// Validate model shapes. Returns empty string if ok, error message otherwise.
std::string validate_model(const FeedForwardNet& net, int expected_input, int expected_output);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_ML_RUNTIME_HPP
