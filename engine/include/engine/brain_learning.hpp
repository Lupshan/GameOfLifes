#ifndef GAMEOFLIFES_ENGINE_BRAIN_LEARNING_HPP
#define GAMEOFLIFES_ENGINE_BRAIN_LEARNING_HPP

#include "engine/brain_ml.hpp"
#include "engine/ml_runtime.hpp"

namespace gol {

// Tier 3: a learning brain that updates its weights via perturb-and-measure.
// Each tick: run inference, record energy before/after, periodically perturb
// weights and keep changes that improve the reward (energy delta).
class LearningBrain : public Brain {
  public:
    LearningBrain(const FeedForwardNet& net, double learning_rate, int update_interval);
    void tick(Agent& agent, World& world) override;

  private:
    FeedForwardNet net_;
    MlBrain inference_brain_;
    double learning_rate_;
    int update_interval_;
    int tick_counter_ = 0;
    int prev_energy_ = 0;
    FeedForwardNet candidate_net_;
    bool has_candidate_ = false;

    void perturb_weights(Rng& rng);
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BRAIN_LEARNING_HPP
