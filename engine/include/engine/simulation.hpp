#ifndef GAMEOFLIFES_ENGINE_SIMULATION_HPP
#define GAMEOFLIFES_ENGINE_SIMULATION_HPP

#include "engine/lineage_log.hpp"
#include "engine/world.hpp"

namespace gol {

class Simulation {
  public:
    explicit Simulation(World& world);
    Simulation(World& world, LineageLog& log);

    void tick();
    void run(int n_ticks);

  private:
    World& world_;
    LineageLog* log_ = nullptr;
};

void step_agent(Agent& agent, World& world);
void reproduce_agents(World& world, LineageLog* log);
void respawn_food(World& world);
void reap_dead(World& world, LineageLog* log);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_SIMULATION_HPP
