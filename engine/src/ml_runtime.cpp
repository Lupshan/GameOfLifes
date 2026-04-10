#include "engine/ml_runtime.hpp"

#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace gol {

static Activation parse_activation(const std::string& s) {
    if (s == "relu") {
        return Activation::Relu;
    }
    if (s == "sigmoid") {
        return Activation::Sigmoid;
    }
    if (s == "none" || s.empty()) {
        return Activation::None;
    }
    throw std::runtime_error("ml_runtime: unknown activation: " + s);
}

FeedForwardNet load_model(const std::string& json_str) {
    auto j = nlohmann::json::parse(json_str);

    FeedForwardNet net;
    auto& layers_json = j.at("layers");

    for (const auto& lj : layers_json) {
        Layer layer;
        layer.input_size = lj.at("input_size").get<int>();
        layer.output_size = lj.at("output_size").get<int>();
        layer.activation = parse_activation(lj.value("activation", "none"));

        auto& w = lj.at("weights");
        int expected_weights = layer.input_size * layer.output_size;
        if (static_cast<int>(w.size()) != expected_weights) {
            throw std::runtime_error("ml_runtime: weight count mismatch");
        }
        layer.weights.reserve(static_cast<std::size_t>(expected_weights));
        for (const auto& val : w) {
            layer.weights.push_back(val.get<float>());
        }

        auto& b = lj.at("biases");
        if (static_cast<int>(b.size()) != layer.output_size) {
            throw std::runtime_error("ml_runtime: bias count mismatch");
        }
        layer.biases.reserve(static_cast<std::size_t>(layer.output_size));
        for (const auto& val : b) {
            layer.biases.push_back(val.get<float>());
        }

        net.layers.push_back(layer);
    }

    if (!net.layers.empty()) {
        net.input_size = net.layers.front().input_size;
        net.output_size = net.layers.back().output_size;
    }

    return net;
}

std::vector<float> infer(const FeedForwardNet& net, const std::vector<float>& input) {
    std::vector<float> current = input;

    for (const auto& layer : net.layers) {
        std::vector<float> output(static_cast<std::size_t>(layer.output_size), 0.0f);

        // Matrix multiply: output = weights * current + biases.
        for (int o = 0; o < layer.output_size; ++o) {
            float sum = layer.biases[static_cast<std::size_t>(o)];
            for (int i = 0; i < layer.input_size; ++i) {
                sum += layer.weights[static_cast<std::size_t>(o * layer.input_size + i)] *
                       current[static_cast<std::size_t>(i)];
            }

            // Activation.
            switch (layer.activation) {
            case Activation::Relu:
                output[static_cast<std::size_t>(o)] = std::max(0.0f, sum);
                break;
            case Activation::Sigmoid:
                output[static_cast<std::size_t>(o)] = 1.0f / (1.0f + std::exp(-sum));
                break;
            case Activation::None:
                output[static_cast<std::size_t>(o)] = sum;
                break;
            }
        }

        current = output;
    }

    return current;
}

int argmax(const std::vector<float>& v) {
    if (v.empty()) {
        return -1;
    }
    return static_cast<int>(std::distance(v.begin(), std::max_element(v.begin(), v.end())));
}

std::string validate_model(const FeedForwardNet& net, int expected_input, int expected_output) {
    if (net.layers.empty()) {
        return "model has no layers";
    }
    if (net.input_size != expected_input) {
        return "expected input size " + std::to_string(expected_input) + ", got " +
               std::to_string(net.input_size);
    }
    if (net.output_size != expected_output) {
        return "expected output size " + std::to_string(expected_output) + ", got " +
               std::to_string(net.output_size);
    }
    // Check layer chain.
    for (std::size_t i = 1; i < net.layers.size(); ++i) {
        if (net.layers[i].input_size != net.layers[i - 1].output_size) {
            return "layer " + std::to_string(i) + " input mismatch";
        }
    }
    return "";
}

} // namespace gol
