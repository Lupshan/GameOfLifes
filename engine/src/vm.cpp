#include "engine/vm.hpp"

#include <cstring>

namespace gol {

Vm::Vm() {
    stack_.fill(0);
    locals_.fill(0);
}

void Vm::load(const Bytecode& bc) {
    bc_ = &bc;
    pc_ = 0;
    sp_ = 0;
    call_depth_ = 0;
    halted_ = false;
    last_error_ = VmStatus::Ok;
    stack_.fill(0);
    locals_.fill(0);
}

void Vm::set_intrinsic_handler(IntrinsicHandler* handler) {
    intrinsic_handler_ = handler;
}

bool Vm::can_push() const {
    return sp_ < VM_MAX_STACK;
}

bool Vm::can_pop() const {
    return sp_ > 0;
}

void Vm::reset_for_tick() {
    pc_ = 0;
    sp_ = 0;
    call_depth_ = 0;
    halted_ = false;
    last_error_ = VmStatus::Ok;
}

void Vm::push(std::int32_t value) {
    stack_[static_cast<std::size_t>(sp_)] = value;
    ++sp_;
}

std::int32_t Vm::pop() {
    --sp_;
    return stack_[static_cast<std::size_t>(sp_)];
}

std::uint8_t Vm::read_u8() {
    return bc_->code[pc_++];
}

std::uint16_t Vm::read_u16() {
    std::uint16_t lo = bc_->code[pc_];
    std::uint16_t hi = bc_->code[pc_ + 1];
    pc_ += 2;
    return lo | (hi << 8);
}

std::int32_t Vm::read_i16() {
    std::uint16_t u = read_u16();
    // Sign-extend from 16 bits.
    if (u & 0x8000) {
        return static_cast<std::int32_t>(u | 0xFFFF0000u);
    }
    return static_cast<std::int32_t>(u);
}

std::int32_t Vm::read_i32() {
    std::uint32_t b0 = bc_->code[pc_];
    std::uint32_t b1 = bc_->code[pc_ + 1];
    std::uint32_t b2 = bc_->code[pc_ + 2];
    std::uint32_t b3 = bc_->code[pc_ + 3];
    pc_ += 4;
    std::uint32_t u = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    std::int32_t result;
    std::memcpy(&result, &u, sizeof(result));
    return result;
}

VmStatus Vm::step() {
    if (halted_) {
        return VmStatus::Halted;
    }
    if (bc_ == nullptr || pc_ >= bc_->code.size()) {
        halted_ = true;
        return VmStatus::Halted;
    }

    std::uint8_t opcode = bc_->code[pc_++];

    switch (opcode) {
    case op::HALT:
        halted_ = true;
        return VmStatus::Halted;

    case op::NOP:
        return VmStatus::Ok;

    case op::PUSH_INT: {
        if (sp_ >= VM_MAX_STACK) {
            return VmStatus::StackOverflow;
        }
        if (pc_ + 4 > bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        std::int32_t val = read_i32();
        push(val);
        return VmStatus::Ok;
    }

    case op::PUSH_BOOL: {
        if (sp_ >= VM_MAX_STACK) {
            return VmStatus::StackOverflow;
        }
        if (pc_ >= bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        std::uint8_t val = read_u8();
        push(val != 0 ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::POP: {
        if (sp_ <= 0) {
            return VmStatus::StackUnderflow;
        }
        pop();
        return VmStatus::Ok;
    }

    case op::DUP: {
        if (sp_ <= 0) {
            return VmStatus::StackUnderflow;
        }
        if (sp_ >= VM_MAX_STACK) {
            return VmStatus::StackOverflow;
        }
        push(stack_[static_cast<std::size_t>(sp_ - 1)]);
        return VmStatus::Ok;
    }

    case op::SWAP: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t tmp = stack_[static_cast<std::size_t>(sp_ - 1)];
        stack_[static_cast<std::size_t>(sp_ - 1)] = stack_[static_cast<std::size_t>(sp_ - 2)];
        stack_[static_cast<std::size_t>(sp_ - 2)] = tmp;
        return VmStatus::Ok;
    }

    case op::ADD: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a + b);
        return VmStatus::Ok;
    }

    case op::SUB: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a - b);
        return VmStatus::Ok;
    }

    case op::MUL: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a * b);
        return VmStatus::Ok;
    }

    case op::DIV: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        if (b == 0) {
            return VmStatus::DivisionByZero;
        }
        push(a / b);
        return VmStatus::Ok;
    }

    case op::MOD: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        if (b == 0) {
            return VmStatus::DivisionByZero;
        }
        push(a % b);
        return VmStatus::Ok;
    }

    case op::NEG: {
        if (sp_ < 1) {
            return VmStatus::StackUnderflow;
        }
        stack_[static_cast<std::size_t>(sp_ - 1)] = -stack_[static_cast<std::size_t>(sp_ - 1)];
        return VmStatus::Ok;
    }

    case op::EQ: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a == b ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::LT: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a < b ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::GT: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push(a > b ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::AND: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push((a != 0 && b != 0) ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::OR: {
        if (sp_ < 2) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t b = pop();
        std::int32_t a = pop();
        push((a != 0 || b != 0) ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::NOT: {
        if (sp_ < 1) {
            return VmStatus::StackUnderflow;
        }
        std::int32_t a = pop();
        push(a == 0 ? 1 : 0);
        return VmStatus::Ok;
    }

    case op::JMP: {
        if (pc_ + 2 > bc_->code.size()) {
            return VmStatus::OutOfBoundsJump;
        }
        std::int32_t offset = read_i16();
        std::int64_t target = static_cast<std::int64_t>(pc_) + offset - 2;
        // -2 because PC already advanced past the operand, but offset is relative to opcode+operand
        // Actually, let's define: JMP offset means PC = PC_after_operand + offset - 2
        // Simpler: target = PC_before_operand + offset
        target = static_cast<std::int64_t>(pc_) - 2 + offset;
        if (target < 0 || target > static_cast<std::int64_t>(bc_->code.size())) {
            return VmStatus::OutOfBoundsJump;
        }
        pc_ = static_cast<std::uint32_t>(target);
        return VmStatus::Ok;
    }

    case op::JMP_IF_FALSE: {
        if (sp_ < 1) {
            return VmStatus::StackUnderflow;
        }
        if (pc_ + 2 > bc_->code.size()) {
            return VmStatus::OutOfBoundsJump;
        }
        std::int32_t offset = read_i16();
        std::int32_t cond = pop();
        if (cond == 0) {
            std::int64_t target = static_cast<std::int64_t>(pc_) - 2 + offset;
            if (target < 0 || target > static_cast<std::int64_t>(bc_->code.size())) {
                return VmStatus::OutOfBoundsJump;
            }
            pc_ = static_cast<std::uint32_t>(target);
        }
        return VmStatus::Ok;
    }

    case op::CALL: {
        if (call_depth_ >= VM_MAX_CALL_DEPTH) {
            return VmStatus::CallDepthExceeded;
        }
        if (pc_ + 2 > bc_->code.size()) {
            return VmStatus::OutOfBoundsJump;
        }
        std::uint16_t addr = read_u16();
        if (addr > bc_->code.size()) {
            return VmStatus::OutOfBoundsJump;
        }
        call_stack_[static_cast<std::size_t>(call_depth_)] = CallFrame{pc_};
        ++call_depth_;
        pc_ = addr;
        return VmStatus::Ok;
    }

    case op::RETURN: {
        if (call_depth_ <= 0) {
            // RETURN at top level = halt.
            halted_ = true;
            return VmStatus::Halted;
        }
        --call_depth_;
        pc_ = call_stack_[static_cast<std::size_t>(call_depth_)].return_pc;
        return VmStatus::Ok;
    }

    case op::LOAD_LOCAL: {
        if (pc_ >= bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        if (sp_ >= VM_MAX_STACK) {
            return VmStatus::StackOverflow;
        }
        std::uint8_t idx = read_u8();
        if (idx >= bc_->local_count) {
            return VmStatus::OutOfBoundsLocal;
        }
        push(locals_[idx]);
        return VmStatus::Ok;
    }

    case op::STORE_LOCAL: {
        if (pc_ >= bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        if (sp_ < 1) {
            return VmStatus::StackUnderflow;
        }
        std::uint8_t idx = read_u8();
        if (idx >= bc_->local_count) {
            return VmStatus::OutOfBoundsLocal;
        }
        locals_[idx] = pop();
        return VmStatus::Ok;
    }

    case op::LOAD_CONST: {
        if (pc_ + 2 > bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        if (sp_ >= VM_MAX_STACK) {
            return VmStatus::StackOverflow;
        }
        std::uint16_t idx = read_u16();
        if (idx >= bc_->constants.size()) {
            return VmStatus::InvalidOpcode;
        }
        if (auto* val = std::get_if<std::int32_t>(&bc_->constants[idx])) {
            push(*val);
        } else {
            // String constants can't be pushed to the int stack.
            return VmStatus::InvalidOpcode;
        }
        return VmStatus::Ok;
    }

    // Agent intrinsics — dispatched through handler.
    case op::PERCEIVE:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_perceive(*this);
    case op::LOOK:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_look(*this);
    case op::MOVE:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_move(*this);
    case op::TURN_LEFT:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_turn_left(*this);
    case op::TURN_RIGHT:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_turn_right(*this);
    case op::EAT:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_eat(*this);
    case op::DRINK:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_drink(*this);
    case op::REPRODUCE:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_reproduce(*this);
    case op::MY_ENERGY:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_my_energy(*this);
    case op::MY_HYDRATION:
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        return intrinsic_handler_->handle_my_hydration(*this);
    case op::MY_TRAIT: {
        if (intrinsic_handler_ == nullptr) {
            return VmStatus::UnimplementedIntrinsic;
        }
        if (pc_ >= bc_->code.size()) {
            return VmStatus::InvalidOpcode;
        }
        std::uint8_t trait_id = read_u8();
        return intrinsic_handler_->handle_my_trait(*this, trait_id);
    }

    default:
        return VmStatus::InvalidOpcode;
    }
}

VmStatus Vm::run(int budget) {
    int remaining = budget;
    while (remaining > 0) {
        // Peek at the next opcode to determine cost before executing.
        std::uint8_t next_op = (bc_ != nullptr && pc_ < bc_->code.size()) ? bc_->code[pc_] : 0;
        bool is_intrinsic = (next_op >= 0x80 && next_op <= 0x8F);
        int cost = is_intrinsic ? INTRINSIC_COST : 1;

        if (remaining < cost) {
            return VmStatus::BudgetExhausted;
        }

        VmStatus status = step();
        remaining -= cost;

        if (status != VmStatus::Ok) {
            return status;
        }
    }
    return VmStatus::BudgetExhausted;
}

std::int32_t Vm::stack_top() const {
    return stack_[static_cast<std::size_t>(sp_ - 1)];
}

int Vm::stack_size() const {
    return sp_;
}

std::int32_t Vm::local(int index) const {
    return locals_[static_cast<std::size_t>(index)];
}

void Vm::set_local(int index, std::int32_t value) {
    locals_[static_cast<std::size_t>(index)] = value;
}

std::uint32_t Vm::pc() const {
    return pc_;
}

} // namespace gol
