#ifndef GAMEOFLIFES_ENGINE_BRAIN_ML_HPP
#define GAMEOFLIFES_ENGINE_BRAIN_ML_HPP

#include "engine/brain.hpp"
#include "engine/ml_runtime.hpp"

namespace gol {

// Brain that runs a feed-forward neural network.
class MlBrain : public Brain {
  public:
    explicit MlBrain(const FeedForwardNet& net);
    void tick(Agent& agent, World& world) override;

  private:
    FeedForwardNet net_;

    // Encode the agent's perception into a float vector.
    std::vector<float> encode_perception(const Agent& agent, const World& world) const;

    // Execute the action corresponding to the argmax output.
    void execute_action(int action, Agent& agent, World& world) const;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BRAIN_ML_HPP
