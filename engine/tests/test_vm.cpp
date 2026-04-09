#include "engine/bytecode.hpp"
#include "engine/vm.hpp"

#include <cstdint>
#include <cstring>
#include <doctest/doctest.h>
#include <vector>

// Helper: encode i32 as 4 little-endian bytes.
static void push_i32(std::vector<std::uint8_t>& code, std::int32_t val) {
    std::uint32_t u;
    std::memcpy(&u, &val, 4);
    code.push_back(static_cast<std::uint8_t>(u & 0xFF));
    code.push_back(static_cast<std::uint8_t>((u >> 8) & 0xFF));
    code.push_back(static_cast<std::uint8_t>((u >> 16) & 0xFF));
    code.push_back(static_cast<std::uint8_t>((u >> 24) & 0xFF));
}

// Helper: encode i16 as 2 little-endian bytes.
static void push_i16(std::vector<std::uint8_t>& code, std::int16_t val) {
    std::uint16_t u;
    std::memcpy(&u, &val, 2);
    code.push_back(static_cast<std::uint8_t>(u & 0xFF));
    code.push_back(static_cast<std::uint8_t>((u >> 8) & 0xFF));
}

// ---- HALT ----

TEST_CASE("VM: HALT stops execution") {
    auto bc = gol::make_bytecode({gol::op::HALT});
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(100);
    CHECK(status == gol::VmStatus::Halted);
}

// ---- PUSH_INT + stack ----

TEST_CASE("VM: PUSH_INT pushes value") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 42);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_size() == 1);
    CHECK(vm.stack_top() == 42);
}

// ---- PUSH_BOOL ----

TEST_CASE("VM: PUSH_BOOL") {
    std::vector<std::uint8_t> code = {gol::op::PUSH_BOOL, 1, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}

// ---- POP ----

TEST_CASE("VM: POP") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 10);
    code.push_back(gol::op::POP);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_size() == 0);
}

// ---- DUP ----

TEST_CASE("VM: DUP") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 7);
    code.push_back(gol::op::DUP);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_size() == 2);
    CHECK(vm.stack_top() == 7);
}

// ---- SWAP ----

TEST_CASE("VM: SWAP") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 1);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 2);
    code.push_back(gol::op::SWAP);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}

// ---- ADD ----

TEST_CASE("VM: ADD") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 3);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 4);
    code.push_back(gol::op::ADD);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 7);
}

// ---- SUB ----

TEST_CASE("VM: SUB") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 10);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 3);
    code.push_back(gol::op::SUB);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 7);
}

// ---- MUL ----

TEST_CASE("VM: MUL") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 6);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 7);
    code.push_back(gol::op::MUL);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 42);
}

// ---- DIV ----

TEST_CASE("VM: DIV") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 20);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 3);
    code.push_back(gol::op::DIV);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 6);
}

TEST_CASE("VM: DIV by zero") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 10);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 0);
    code.push_back(gol::op::DIV);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(100);
    CHECK(status == gol::VmStatus::DivisionByZero);
}

// ---- NEG ----

TEST_CASE("VM: NEG") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 42);
    code.push_back(gol::op::NEG);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == -42);
}

// ---- EQ, LT, GT ----

TEST_CASE("VM: EQ true") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 5);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 5);
    code.push_back(gol::op::EQ);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}

TEST_CASE("VM: LT") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 3);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 5);
    code.push_back(gol::op::LT);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}

// ---- NOT ----

TEST_CASE("VM: NOT") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 0);
    code.push_back(gol::op::NOT);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}

// ---- LOAD_LOCAL / STORE_LOCAL ----

TEST_CASE("VM: local variables") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 99);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(0);
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(0);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code, 1);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 99);
}

// ---- LOAD_CONST ----

TEST_CASE("VM: LOAD_CONST") {
    gol::Bytecode bc;
    bc.local_count = 0;
    bc.constants.push_back(std::int32_t(777));

    std::vector<std::uint8_t> code;
    code.push_back(gol::op::LOAD_CONST);
    code.push_back(0);
    code.push_back(0); // index 0, little-endian u16
    code.push_back(gol::op::HALT);
    bc.code = code;

    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 777);
}

// ---- JMP ----

TEST_CASE("VM: JMP forward skips instructions") {
    // JMP +3 skips the next PUSH_INT (5 bytes: opcode + 4 byte int), but
    // offset is from the operand position. Let me think...
    // Layout: [JMP, lo, hi, PUSH_INT, i32(99), PUSH_INT, i32(42), HALT]
    // Positions: 0=JMP, 1-2=offset, 3=PUSH_INT, 4-7=99, 8=PUSH_INT, 9-12=42, 13=HALT
    // After reading JMP operand, pc=3. We want to jump to position 8.
    // offset = target - (pc - 2) = 8 - (3-2) = 8 - 1 = 7
    // Wait let me re-check the impl: target = pc - 2 + offset, where pc is after reading operand
    // So: target = 3 - 2 + offset = 1 + offset. We want target=8, so offset=7.
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::JMP);
    push_i16(code, 7); // offset = 7
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 99); // should be skipped
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 42); // should execute
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_size() == 1);
    CHECK(vm.stack_top() == 42);
}

// ---- JMP_IF_FALSE ----

TEST_CASE("VM: JMP_IF_FALSE branches when false") {
    // Push 0, JMP_IF_FALSE to skip a push of 99.
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 0); // false condition
    code.push_back(gol::op::JMP_IF_FALSE);
    push_i16(code, 7); // skip next push
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 99); // skipped
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 42); // reached
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_size() == 1);
    CHECK(vm.stack_top() == 42);
}

// ---- CALL / RETURN ----

TEST_CASE("VM: CALL and RETURN") {
    // Layout:
    // 0: CALL 7         (3 bytes: opcode + u16 addr=7)
    // 3: HALT           (1 byte) — return here is wrong, we want to land after CALL
    // Wait, CALL pushes return_pc = pc after reading operand = 3
    // So after RETURN, pc = 3 which is HALT. Let's do:
    // 0: CALL addr=4    (jump to the function at byte 4)
    // 3: HALT
    // 4: PUSH_INT 99
    // 9: RETURN

    std::vector<std::uint8_t> code;
    code.push_back(gol::op::CALL);
    push_i16(code, 4);                 // call function at byte 4
    code.push_back(gol::op::HALT);     // byte 3
    code.push_back(gol::op::PUSH_INT); // byte 4 (function start)
    push_i32(code, 99);
    code.push_back(gol::op::RETURN);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(100);
    CHECK(status == gol::VmStatus::Halted);
    CHECK(vm.stack_size() == 1);
    CHECK(vm.stack_top() == 99);
}

// ---- Budget exhaustion ----

TEST_CASE("VM: budget exhaustion on infinite loop") {
    // Infinite loop: JMP -3 (back to itself).
    // Position 0: JMP, offset bytes at 1-2
    // After read, pc=3, target = 3-2+(-3) = -2... hmm that's wrong.
    // target = pc-2 + offset = 3-2+offset. For target=0: offset = -1.
    // Wait: offset calculation: target = (pc after reading operand) - 2 + offset
    // pc after reading operand at position 0 is 3 (0 for opcode, 1-2 for operand)
    // target = 3 - 2 + offset = 1 + offset. For target=0: offset=-1.

    std::vector<std::uint8_t> code;
    code.push_back(gol::op::JMP);
    push_i16(code, -1); // jump back to start

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(1000);
    CHECK(status == gol::VmStatus::BudgetExhausted);
}

// ---- Stack underflow ----

TEST_CASE("VM: stack underflow") {
    auto bc = gol::make_bytecode({gol::op::POP});
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(100);
    CHECK(status == gol::VmStatus::StackUnderflow);
}

// ---- Invalid opcode ----

TEST_CASE("VM: invalid opcode") {
    auto bc = gol::make_bytecode({0xFE}); // not a valid opcode
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(100);
    CHECK(status == gol::VmStatus::InvalidOpcode);
}

// ---- Sample program: factorial(5) = 120 ----

TEST_CASE("VM: factorial(5)") {
    // Compute 5! using locals.
    // local[0] = n (counter), local[1] = result
    // n = 5, result = 1
    // while n > 1: result *= n; n -= 1
    // push result

    std::vector<std::uint8_t> code;

    // Initialize: n = 5
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 5);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(0); // local[0] = 5

    // result = 1
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 1);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(1); // local[1] = 1

    // Loop start (byte 12):
    std::size_t loop_start = code.size();

    // if n <= 1, jump to end
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(0); // push n
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 1);
    code.push_back(gol::op::GT); // n > 1?
    code.push_back(gol::op::JMP_IF_FALSE);
    std::size_t jmp_false_operand = code.size();
    push_i16(code, 0); // placeholder — will patch

    // result = result * n
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(1); // push result
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(0); // push n
    code.push_back(gol::op::MUL);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(1); // local[1] = result * n

    // n = n - 1
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(0);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 1);
    code.push_back(gol::op::SUB);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(0);

    // Jump back to loop start
    code.push_back(gol::op::JMP);
    std::int16_t back_offset = static_cast<std::int16_t>(static_cast<int>(loop_start) -
                                                         static_cast<int>(code.size()) - 2 + 2);
    // target = pc_after - 2 + offset => offset = target - pc_after + 2
    // pc_after = code.size() + 2 (after reading the i16)
    // target = loop_start
    // offset = loop_start - (code.size() + 2) + 2 = loop_start - code.size()
    back_offset = static_cast<std::int16_t>(static_cast<int>(loop_start) -
                                            static_cast<int>(code.size()) - 2 + 2);
    push_i16(code, back_offset);

    // End (here the JMP_IF_FALSE should land):
    std::size_t end_pos = code.size();

    // Patch JMP_IF_FALSE offset
    // From JMP_IF_FALSE: pc after reading operand = jmp_false_operand + 2
    // target = end_pos
    // offset = target - (pc_after - 2) = end_pos - jmp_false_operand
    std::int16_t fwd_offset =
        static_cast<std::int16_t>(static_cast<int>(end_pos) - static_cast<int>(jmp_false_operand));
    std::uint16_t u;
    std::memcpy(&u, &fwd_offset, 2);
    code[jmp_false_operand] = static_cast<std::uint8_t>(u & 0xFF);
    code[jmp_false_operand + 1] = static_cast<std::uint8_t>((u >> 8) & 0xFF);

    // Push result
    code.push_back(gol::op::LOAD_LOCAL);
    code.push_back(1);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code, 2);
    gol::Vm vm;
    vm.load(bc);
    gol::VmStatus status = vm.run(1000);
    CHECK(status == gol::VmStatus::Halted);
    CHECK(vm.stack_top() == 120);
}

// ---- Locals persist across ticks ----

TEST_CASE("VM: locals persist across reset_for_tick") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 42);
    code.push_back(gol::op::STORE_LOCAL);
    code.push_back(0);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code, 1);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);

    // Reset for next tick.
    vm.reset_for_tick();
    CHECK(vm.stack_size() == 0);
    CHECK(vm.local(0) == 42); // persists!
}

// ---- NOP ----

TEST_CASE("VM: NOP does nothing") {
    std::vector<std::uint8_t> code;
    code.push_back(gol::op::NOP);
    code.push_back(gol::op::PUSH_INT);
    push_i32(code, 1);
    code.push_back(gol::op::HALT);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.run(100);
    CHECK(vm.stack_top() == 1);
}
