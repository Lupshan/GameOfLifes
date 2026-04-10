#include "engine/brain_bytecode.hpp"

namespace gol {

BytecodeBrain::BytecodeBrain(const Bytecode& bc, int budget) : bc_(bc), budget_(budget) {
}

void BytecodeBrain::tick(Agent& agent, World& world) {
    if (!loaded_) {
        vm_.load(bc_);
        loaded_ = true;
    } else {
        vm_.reset_for_tick();
    }

    // Find agent index in the world's agent vector.
    std::size_t agent_index = 0;
    for (std::size_t i = 0; i < world.agents().size(); ++i) {
        if (world.agents()[i].id == agent.id) {
            agent_index = i;
            break;
        }
    }

    AgentIntrinsicHandler handler(agent_index, world);
    vm_.set_intrinsic_handler(&handler);
    vm_.run(budget_);
}

} // namespace gol
