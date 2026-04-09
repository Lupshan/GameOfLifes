# Bytecode opcodes — v1

All opcodes are 1-byte wide. Operands follow the opcode in the instruction stream, little-endian. The VM is stack-based: operands are popped from and results pushed to the operand stack.

## Notation

- `[operand]` = inline operand in the bytecode stream (not from the stack).
- `a, b → c` = pops `a` then `b`, pushes `c`.
- Stack grows upward; top-of-stack is rightmost.

## Stack manipulation

| Code | Name       | Operands          | Stack effect     | Description                      |
|------|------------|-------------------|------------------|----------------------------------|
| 0x01 | PUSH_INT   | i32 (4 bytes)     | → value          | Push a 32-bit signed integer.    |
| 0x02 | PUSH_BOOL  | u8 (1 byte)       | → value          | Push a boolean (0 or 1).         |
| 0x03 | POP        | —                 | value →          | Discard top of stack.            |
| 0x04 | DUP        | —                 | a → a, a         | Duplicate top of stack.          |
| 0x05 | SWAP       | —                 | a, b → b, a      | Swap top two values.             |

## Arithmetic (int32 only)

| Code | Name | Operands | Stack effect    | Description                                   |
|------|------|----------|-----------------|-----------------------------------------------|
| 0x10 | ADD  | —        | a, b → a+b      | Integer addition.                             |
| 0x11 | SUB  | —        | a, b → a-b      | Integer subtraction.                          |
| 0x12 | MUL  | —        | a, b → a*b      | Integer multiplication.                       |
| 0x13 | DIV  | —        | a, b → a/b      | Integer division (truncates toward zero). Division by zero → runtime error, agent halts. |
| 0x14 | MOD  | —        | a, b → a%b      | Integer modulo. Zero divisor → runtime error. |
| 0x15 | NEG  | —        | a → -a          | Negate top of stack.                          |

## Comparison and logic

| Code | Name | Operands | Stack effect    | Description                    |
|------|------|----------|-----------------|--------------------------------|
| 0x20 | EQ   | —        | a, b → (a==b)   | Push 1 if equal, 0 otherwise.  |
| 0x21 | LT   | —        | a, b → (a<b)    | Push 1 if a < b, 0 otherwise.  |
| 0x22 | GT   | —        | a, b → (a>b)    | Push 1 if a > b, 0 otherwise.  |
| 0x23 | AND  | —        | a, b → (a&&b)   | Logical AND (truthy = nonzero).|
| 0x24 | OR   | —        | a, b → (a\|\|b) | Logical OR (truthy = nonzero). |
| 0x25 | NOT  | —        | a → (!a)        | Logical NOT.                   |

## Control flow

| Code | Name          | Operands          | Stack effect | Description                                                       |
|------|---------------|-------------------|--------------|-------------------------------------------------------------------|
| 0x30 | JMP           | offset: i16       | —            | Unconditional jump. `PC += offset` (relative, signed).            |
| 0x31 | JMP_IF_FALSE  | offset: i16       | cond →       | Pop condition; jump if falsy (0).                                 |
| 0x32 | CALL          | addr: u16         | — → (frame)  | Push call frame, jump to absolute address `addr`.                 |
| 0x33 | RETURN        | —                 | (frame) →    | Pop call frame, resume at return address.                         |

## Local variables

| Code | Name         | Operands        | Stack effect   | Description                                 |
|------|--------------|-----------------|----------------|---------------------------------------------|
| 0x40 | LOAD_LOCAL   | index: u8       | → value        | Push local variable at `index` onto stack.  |
| 0x41 | STORE_LOCAL  | index: u8       | value →        | Pop stack into local variable at `index`.   |

## Constants (from constant pool)

| Code | Name         | Operands        | Stack effect   | Description                                 |
|------|--------------|-----------------|----------------|---------------------------------------------|
| 0x50 | LOAD_CONST   | index: u16      | → value        | Push constant from pool at `index`.         |

## Agent intrinsics

These are the agent-world interface. Each costs instruction budget (see `semantics.md`). Finalized in step 14; listed here as placeholders with tentative signatures.

| Code | Name       | Operands | Stack effect       | Description                                     |
|------|------------|----------|--------------------|-------------------------------------------------|
| 0x80 | PERCEIVE   | —        | → terrain, food, water, agents_here | Perceive current cell contents. Pushes 4 values. |
| 0x81 | LOOK       | —        | → terrain, food, water, agents_here | Perceive cell ahead (costs ticks). Pushes 4 values. |
| 0x82 | MOVE       | —        | →                  | Move forward one cell.                           |
| 0x83 | TURN_LEFT  | —        | →                  | Turn 90° counter-clockwise.                      |
| 0x84 | TURN_RIGHT | —        | →                  | Turn 90° clockwise.                              |
| 0x85 | EAT        | —        | → gained           | Eat food on current cell. Pushes energy gained.  |
| 0x86 | DRINK      | —        | → gained           | Drink water on current cell. Pushes hydration gained. |
| 0x87 | REPRODUCE  | —        | → success          | Attempt reproduction. Pushes 1 if successful, 0 if not. |
| 0x88 | MY_ENERGY  | —        | → value            | Push agent's current energy.                     |
| 0x89 | MY_HYDRATION| —       | → value            | Push agent's current hydration.                  |
| 0x8A | MY_TRAIT   | trait_id: u8 | → value        | Push decoded value of trait `trait_id`.           |

## Special

| Code | Name | Operands | Stack effect | Description           |
|------|------|----------|--------------|-----------------------|
| 0x00 | HALT | —        | —            | End execution for this tick. |
| 0xFF | NOP  | —        | —            | No operation.         |
