#ifndef GAMEOFLIFES_ENGINE_SIMULATION_HPP
#define GAMEOFLIFES_ENGINE_SIMULATION_HPP

#include "engine/world.hpp"

namespace gol {

class Simulation {
  public:
    explicit Simulation(World& world);

    void tick();
    void run(int n_ticks);

  private:
    World& world_;
};

void step_agent(Agent& agent, World& world);
void respawn_food(World& world);
void reap_dead(World& world);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_SIMULATION_HPP
