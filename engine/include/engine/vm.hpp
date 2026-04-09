#ifndef GAMEOFLIFES_ENGINE_VM_HPP
#define GAMEOFLIFES_ENGINE_VM_HPP

#include "engine/bytecode.hpp"

#include <array>
#include <cstdint>

namespace gol {

static constexpr int VM_MAX_STACK = 256;
static constexpr int VM_MAX_LOCALS = 256;
static constexpr int VM_MAX_CALL_DEPTH = 32;
static constexpr int INTRINSIC_COST = 10;

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

class Vm;

// Abstract interface for handling agent intrinsic opcodes.
// The simulation provides a concrete implementation bound to a specific agent.
class IntrinsicHandler {
  public:
    virtual ~IntrinsicHandler() = default;

    // Each returns VmStatus::Ok on success, or an error status.
    // The handler pushes results onto the VM stack directly.
    virtual VmStatus handle_perceive(Vm& vm) = 0;
    virtual VmStatus handle_look(Vm& vm) = 0;
    virtual VmStatus handle_move(Vm& vm) = 0;
    virtual VmStatus handle_turn_left(Vm& vm) = 0;
    virtual VmStatus handle_turn_right(Vm& vm) = 0;
    virtual VmStatus handle_eat(Vm& vm) = 0;
    virtual VmStatus handle_drink(Vm& vm) = 0;
    virtual VmStatus handle_reproduce(Vm& vm) = 0;
    virtual VmStatus handle_my_energy(Vm& vm) = 0;
    virtual VmStatus handle_my_hydration(Vm& vm) = 0;
    virtual VmStatus handle_my_trait(Vm& vm, std::uint8_t trait_id) = 0;
};

class Vm {
  public:
    Vm();

    // Load bytecode. Resets PC and stack, preserves nothing.
    void load(const Bytecode& bc);

    // Set the intrinsic handler (must outlive the VM or be reset before destruction).
    void set_intrinsic_handler(IntrinsicHandler* handler);

    // Execute one instruction. Returns status.
    VmStatus step();

    // Run up to `budget` instructions. Returns final status.
    // Intrinsic opcodes cost INTRINSIC_COST from the budget.
    VmStatus run(int budget);

    // Reset for a new tick: clear stack, reset PC to 0. Locals persist.
    void reset_for_tick();

    // Stack manipulation (public for use by IntrinsicHandler).
    void push(std::int32_t value);
    std::int32_t pop();
    bool can_push() const;
    bool can_pop() const;

    // Accessors for testing.
    std::int32_t stack_top() const;
    int stack_size() const;
    std::int32_t local(int index) const;
    void set_local(int index, std::int32_t value);
    std::uint32_t pc() const;

  private:
    std::int32_t read_i16();
    std::int32_t read_i32();
    std::uint8_t read_u8();
    std::uint16_t read_u16();

    const Bytecode* bc_ = nullptr;
    IntrinsicHandler* intrinsic_handler_ = nullptr;
    std::uint32_t pc_ = 0;
    std::array<std::int32_t, VM_MAX_STACK> stack_;
    int sp_ = 0;
    std::array<std::int32_t, VM_MAX_LOCALS> locals_;
    std::array<CallFrame, VM_MAX_CALL_DEPTH> call_stack_;
    int call_depth_ = 0;
    bool halted_ = false;
    VmStatus last_error_ = VmStatus::Ok;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_VM_HPP
