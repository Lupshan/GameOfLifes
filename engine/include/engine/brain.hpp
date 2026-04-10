#ifndef GAMEOFLIFES_ENGINE_BRAIN_HPP
#define GAMEOFLIFES_ENGINE_BRAIN_HPP

#include "engine/agent.hpp"
#include "engine/world.hpp"

namespace gol {

// Abstract brain interface: drives an agent each tick.
class Brain {
  public:
    virtual ~Brain() = default;
    virtual void tick(Agent& agent, World& world) = 0;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BRAIN_HPP
