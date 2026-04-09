#ifndef GAMEOFLIFES_ENGINE_VM_HPP
#define GAMEOFLIFES_ENGINE_VM_HPP

#include "engine/bytecode.hpp"

#include <array>
#include <cstdint>

namespace gol {

static constexpr int VM_MAX_STACK = 256;
static constexpr int VM_MAX_LOCALS = 256;
static constexpr int VM_MAX_CALL_DEPTH = 32;

enum class VmStatus {
    Ok,
    Halted,
    BudgetExhausted,
    StackOverflow,
    StackUnderflow,
    DivisionByZero,
    InvalidOpcode,
    CallDepthExceeded,
    OutOfBoundsLocal,
    OutOfBoundsJump,
    UnimplementedIntrinsic,
};

struct CallFrame {
    std::uint32_t return_pc;
};

class Vm {
  public:
    Vm();

    // Load bytecode. Resets PC and stack, preserves nothing.
    void load(const Bytecode& bc);

    // Execute one instruction. Returns status.
    VmStatus step();

    // Run up to `budget` instructions. Returns final status.
    VmStatus run(int budget);

    // Reset for a new tick: clear stack, reset PC to 0. Locals persist.
    void reset_for_tick();

    // Accessors for testing.
    std::int32_t stack_top() const;
    int stack_size() const;
    std::int32_t local(int index) const;
    void set_local(int index, std::int32_t value);
    std::uint32_t pc() const;

  private:
    void push(std::int32_t value);
    std::int32_t pop();

    std::int32_t read_i16();
    std::int32_t read_i32();
    std::uint8_t read_u8();
    std::uint16_t read_u16();

    const Bytecode* bc_ = nullptr;
    std::uint32_t pc_ = 0;
    std::array<std::int32_t, VM_MAX_STACK> stack_;
    int sp_ = 0; // stack pointer (number of items)
    std::array<std::int32_t, VM_MAX_LOCALS> locals_;
    std::array<CallFrame, VM_MAX_CALL_DEPTH> call_stack_;
    int call_depth_ = 0;
    bool halted_ = false;
    VmStatus last_error_ = VmStatus::Ok;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_VM_HPP
