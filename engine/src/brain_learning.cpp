#include "engine/brain_learning.hpp"

namespace gol {

LearningBrain::LearningBrain(const FeedForwardNet& net, double learning_rate, int update_interval)
    : net_(net), inference_brain_(net), learning_rate_(learning_rate),
      update_interval_(update_interval) {
}

void LearningBrain::perturb_weights(Rng& rng) {
    candidate_net_ = net_;
    for (auto& layer : candidate_net_.layers) {
        for (auto& w : layer.weights) {
            w += static_cast<float>(learning_rate_ * (rng.uniform_real() * 2.0 - 1.0));
        }
        for (auto& b : layer.biases) {
            b += static_cast<float>(learning_rate_ * (rng.uniform_real() * 2.0 - 1.0));
        }
    }
    has_candidate_ = true;
}

void LearningBrain::tick(Agent& agent, World& world) {
    ++tick_counter_;

    // Every update_interval ticks, evaluate and potentially update weights.
    if (tick_counter_ % update_interval_ == 0 && has_candidate_) {
        int reward = agent.energy - prev_energy_;
        if (reward > 0) {
            // Keep the candidate (it improved energy).
            net_ = candidate_net_;
            inference_brain_ = MlBrain(net_);
        }
        has_candidate_ = false;
    }

    // Record energy before action.
    prev_energy_ = agent.energy;

    // Run inference with current net.
    inference_brain_.tick(agent, world);

    // Periodically create a new candidate perturbation.
    if (tick_counter_ % update_interval_ == 1 && !has_candidate_) {
        perturb_weights(world.rng());
        inference_brain_ = MlBrain(candidate_net_);
    }
}

} // namespace gol
