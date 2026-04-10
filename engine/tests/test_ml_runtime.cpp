#include "engine/ml_runtime.hpp"

#include <cmath>
#include <doctest/doctest.h>
#include <string>

// A tiny 2-layer network: 3 inputs → 4 hidden (relu) → 2 outputs (none).
static const std::string TINY_MODEL_JSON = R"({
    "layers": [
        {
            "input_size": 3,
            "output_size": 4,
            "activation": "relu",
            "weights": [1,0,0, 0,1,0, 0,0,1, 1,1,1],
            "biases": [0, 0, 0, -1]
        },
        {
            "input_size": 4,
            "output_size": 2,
            "activation": "none",
            "weights": [1,0,0,0, 0,1,0,0],
            "biases": [0, 0]
        }
    ]
})";

TEST_CASE("load_model parses JSON") {
    auto net = gol::load_model(TINY_MODEL_JSON);
    CHECK(net.layers.size() == 2);
    CHECK(net.input_size == 3);
    CHECK(net.output_size == 2);
}

TEST_CASE("infer produces correct output") {
    auto net = gol::load_model(TINY_MODEL_JSON);
    // Input: [1, 2, 3]
    // Layer 1 (relu):
    //   neuron 0: 1*1 + 0*2 + 0*3 + 0 = 1 → relu → 1
    //   neuron 1: 0*1 + 1*2 + 0*3 + 0 = 2 → relu → 2
    //   neuron 2: 0*1 + 0*2 + 1*3 + 0 = 3 → relu → 3
    //   neuron 3: 1*1 + 1*2 + 1*3 - 1 = 5 → relu → 5
    // Layer 2 (none):
    //   neuron 0: 1*1 + 0*2 + 0*3 + 0*5 + 0 = 1
    //   neuron 1: 0*1 + 1*2 + 0*3 + 0*5 + 0 = 2

    auto output = gol::infer(net, {1.0f, 2.0f, 3.0f});
    CHECK(output.size() == 2);
    CHECK(output[0] == doctest::Approx(1.0f));
    CHECK(output[1] == doctest::Approx(2.0f));
}

TEST_CASE("infer is deterministic") {
    auto net = gol::load_model(TINY_MODEL_JSON);
    auto o1 = gol::infer(net, {1.0f, 2.0f, 3.0f});
    auto o2 = gol::infer(net, {1.0f, 2.0f, 3.0f});
    CHECK(o1 == o2);
}

TEST_CASE("argmax returns correct index") {
    CHECK(gol::argmax({0.1f, 0.9f, 0.5f}) == 1);
    CHECK(gol::argmax({5.0f, 1.0f}) == 0);
    CHECK(gol::argmax({}) == -1);
}

TEST_CASE("validate_model checks shapes") {
    auto net = gol::load_model(TINY_MODEL_JSON);
    CHECK(gol::validate_model(net, 3, 2).empty());
    CHECK_FALSE(gol::validate_model(net, 10, 2).empty());
    CHECK_FALSE(gol::validate_model(net, 3, 5).empty());
}

TEST_CASE("relu activation clamps negative") {
    // Single layer: 1 input → 1 output, weight=-1, bias=0, relu.
    std::string json = R"({
        "layers": [{
            "input_size": 1, "output_size": 1,
            "activation": "relu",
            "weights": [-1], "biases": [0]
        }]
    })";
    auto net = gol::load_model(json);
    auto out = gol::infer(net, {5.0f});
    CHECK(out[0] == doctest::Approx(0.0f)); // -5 → relu → 0
}

TEST_CASE("sigmoid activation") {
    std::string json = R"({
        "layers": [{
            "input_size": 1, "output_size": 1,
            "activation": "sigmoid",
            "weights": [0], "biases": [0]
        }]
    })";
    auto net = gol::load_model(json);
    auto out = gol::infer(net, {0.0f});
    CHECK(out[0] == doctest::Approx(0.5f)); // sigmoid(0) = 0.5
}
