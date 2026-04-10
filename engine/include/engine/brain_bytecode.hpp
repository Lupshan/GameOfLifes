#ifndef GAMEOFLIFES_ENGINE_BRAIN_BYTECODE_HPP
#define GAMEOFLIFES_ENGINE_BRAIN_BYTECODE_HPP

#include "engine/agent_vm.hpp"
#include "engine/brain.hpp"
#include "engine/bytecode.hpp"
#include "engine/vm.hpp"

namespace gol {

// Brain that runs bytecode via the VM.
class BytecodeBrain : public Brain {
  public:
    BytecodeBrain(const Bytecode& bc, int budget);
    void tick(Agent& agent, World& world) override;

  private:
    Bytecode bc_;
    Vm vm_;
    int budget_;
    bool loaded_ = false;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BRAIN_BYTECODE_HPP
