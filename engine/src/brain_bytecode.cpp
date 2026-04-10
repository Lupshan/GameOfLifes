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

    // O(1) agent index lookup via cached map (rebuilt each tick).
    std::size_t agent_index = world.agent_index_by_id(agent.id);
    if (agent_index == SIZE_MAX) {
        return; // agent removed mid-tick
    }

    AgentIntrinsicHandler handler(agent_index, world);
    vm_.set_intrinsic_handler(&handler);
    vm_.run(budget_);
}

} // namespace gol
