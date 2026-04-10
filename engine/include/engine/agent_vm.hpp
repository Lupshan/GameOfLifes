#ifndef GAMEOFLIFES_ENGINE_AGENT_VM_HPP
#define GAMEOFLIFES_ENGINE_AGENT_VM_HPP

#include "engine/agent.hpp"
#include "engine/vm.hpp"
#include "engine/world.hpp"

namespace gol {

// Concrete IntrinsicHandler that binds a VM to a specific agent in a world.
// Uses agent index (not reference) to survive vector reallocation on spawn.
class AgentIntrinsicHandler : public IntrinsicHandler {
  public:
    AgentIntrinsicHandler(std::size_t agent_index, World& world);

    void set_agent_index(std::size_t index);
    Agent& agent();
    bool is_valid() const;

    VmStatus handle_perceive(Vm& vm) override;
    VmStatus handle_look(Vm& vm) override;
    VmStatus handle_move(Vm& vm) override;
    VmStatus handle_turn_left(Vm& vm) override;
    VmStatus handle_turn_right(Vm& vm) override;
    VmStatus handle_eat(Vm& vm) override;
    VmStatus handle_drink(Vm& vm) override;
    VmStatus handle_reproduce(Vm& vm) override;
    VmStatus handle_my_energy(Vm& vm) override;
    VmStatus handle_my_hydration(Vm& vm) override;
    VmStatus handle_my_trait(Vm& vm, std::uint8_t trait_id) override;

  private:
    std::size_t agent_index_;
    World& world_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_AGENT_VM_HPP
