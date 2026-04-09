# GoL Script — Language specification v1

A minimal scripting language for writing bot programs in GameOfLifes. Compiles to the in-house bytecode (see `opcodes.md`). Designed to be learnable in 10 minutes.

## Design principles

- **One numeric type**: `int` (32-bit signed). Booleans are ints (0 = false, nonzero = true).
- **C-like syntax**: braces, semicolons, `if/else`, `while`. Familiar to most programmers.
- **No surprises**: no implicit conversions, no overloading, no inheritance, no closures.
- **Direct mapping to bytecode**: every language feature compiles to existing opcodes. No runtime library.

## Grammar (EBNF)

```ebnf
program        = { function_def } ;

function_def   = "fn" IDENT "(" [ param_list ] ")" block ;
param_list     = IDENT { "," IDENT } ;
block          = "{" { statement } "}" ;

statement      = var_decl
               | assignment
               | if_stmt
               | while_stmt
               | return_stmt
               | expr_stmt ;

var_decl       = "let" IDENT "=" expression ";" ;
assignment     = IDENT "=" expression ";" ;
if_stmt        = "if" "(" expression ")" block [ "else" block ] ;
while_stmt     = "while" "(" expression ")" block ;
return_stmt    = "return" [ expression ] ";" ;
expr_stmt      = expression ";" ;

expression     = or_expr ;
or_expr        = and_expr { "||" and_expr } ;
and_expr       = equality { "&&" equality } ;
equality       = comparison { ( "==" | "!=" ) comparison } ;
comparison     = addition { ( "<" | ">" | "<=" | ">=" ) addition } ;
addition       = multiplication { ( "+" | "-" ) multiplication } ;
multiplication = unary { ( "*" | "/" | "%" ) unary } ;
unary          = [ "-" | "!" ] primary ;
primary        = INT_LITERAL
               | "true" | "false"
               | IDENT
               | IDENT "(" [ arg_list ] ")"
               | "(" expression ")" ;
arg_list       = expression { "," expression } ;

INT_LITERAL    = DIGIT { DIGIT } ;
IDENT          = LETTER { LETTER | DIGIT | "_" } ;
LETTER         = "a".."z" | "A".."Z" | "_" ;
DIGIT          = "0".."9" ;
```

## Comments

```
// Single-line comment (everything until newline).
```

No block comments in v1.

## Types

| Type | Description | Literal examples |
|------|-------------|------------------|
| `int` | 32-bit signed integer | `0`, `42`, `-1` |
| `bool` | Integer alias | `true` (1), `false` (0) |

There is no type annotation syntax — all variables are `int`. Booleans are just ints. Conditions accept any int (0 = false, nonzero = true).

## Variables

```
let x = 10;        // declare and initialize
x = x + 1;         // reassign
```

Variables are local to their enclosing function. Variables declared in a block (`if`, `while`) are still visible in the rest of the function (no block scoping in v1 — simplifies compilation).

All variables persist across ticks (they map to VM locals).

## Control flow

### if / else

```
if (energy() < 30) {
    eat();
} else {
    move();
}
```

### while

```
while (energy() > 50) {
    move();
    eat();
}
```

No `for` loops in v1. Use `while` with a counter.

### Functions

```
fn should_eat() {
    return energy() < 50;
}

fn main() {
    if (should_eat()) {
        eat();
    }
}
```

- `fn main()` is the entry point, called every tick.
- Functions may call other functions (max depth: 32).
- No recursion limit beyond call depth.
- No forward declarations needed — all functions visible to all.

## Operators

### Arithmetic (int → int)

| Op | Description |
|----|-------------|
| `+` | Addition |
| `-` | Subtraction (binary) or negation (unary) |
| `*` | Multiplication |
| `/` | Integer division (truncates toward zero). Division by zero halts the tick. |
| `%` | Modulo. Zero divisor halts the tick. |

### Comparison (int × int → bool)

| Op | Description |
|----|-------------|
| `==` | Equal |
| `!=` | Not equal |
| `<`  | Less than |
| `>`  | Greater than |
| `<=` | Less than or equal |
| `>=` | Greater than or equal |

### Logical (bool × bool → bool)

| Op | Description |
|----|-------------|
| `&&` | Logical AND (short-circuit) |
| `\|\|` | Logical OR (short-circuit) |
| `!`  | Logical NOT (unary) |

Note: short-circuit is a compiler optimization, not required in v1. Both sides may be evaluated.

## Builtin functions (intrinsics)

These map directly to VM intrinsic opcodes.

### Perception

| Function | Returns | Description |
|----------|---------|-------------|
| `perceive()` | `int` terrain, `int` food, `int` water, `int` agents | Perceive current cell. Returns 4 values (use as a statement to discard, or assign each). |
| `look()` | Same as perceive | Perceive cell ahead. |
| `my_energy()` | `int` | Current energy. |
| `my_hydration()` | `int` | Current hydration. |
| `my_trait(id)` | `int` | Decoded trait value. IDs: 0=vision, 1=speed, 2=metabolism, 3=repro_threshold, 4=swim, 5=forage. |

Since the language has no tuples, `perceive()` and `look()` push 4 values onto the stack. The compiler provides convenience access:

```
let terrain = perceive_terrain();   // just the terrain value
let food = perceive_food();         // just the food value
let water = perceive_water();       // just the water value
let agents = perceive_agents();     // just the agent count
```

These are compiler sugar: emit PERCEIVE then pop/reorder to keep only the desired value.

Similarly for `look_terrain()`, `look_food()`, `look_water()`, `look_agents()`.

### Actions

| Function | Returns | Description |
|----------|---------|-------------|
| `move()` | — | Move forward one cell. |
| `turn_left()` | — | Turn 90° counter-clockwise. |
| `turn_right()` | — | Turn 90° clockwise. |
| `eat()` | `int` | Eat food on current cell. Returns energy gained. |
| `drink()` | `int` | Drink water on current cell. Returns hydration gained. |
| `reproduce()` | `int` | Attempt reproduction. Returns 1 on success, 0 on failure. |

## Deliberate non-features (v1)

- **No strings**: no use case in a grid simulation. Debug names are in bytecode metadata.
- **No floats**: the simulation operates on integers. Use fixed-point if needed.
- **No closures / lambdas**: functions are top-level only.
- **No arrays / collections**: agents operate on single values. Could add in v2.
- **No imports / modules**: one file per bot.
- **No global variables**: all state is per-function locals (which persist across ticks).
- **No classes / structs**: unnecessary complexity for v1.
- **No FFI / external calls**: sandboxed by design (ADR 004).

## Sample programs

### 1. Random walker

```
fn main() {
    turn_right();
    move();
    eat();
    drink();
}
```

### 2. Food seeker

```
fn main() {
    let food = look_food();
    if (food > 0) {
        move();
        eat();
    } else {
        turn_right();
    }

    drink();

    if (my_energy() > 100) {
        reproduce();
    }
}
```

### 3. Opportunistic eater

```
fn main() {
    // Eat if food is here.
    let here_food = perceive_food();
    if (here_food > 0) {
        eat();
    }

    // Drink if water is here.
    let here_water = perceive_water();
    if (here_water > 0) {
        drink();
    }

    // Look ahead for food.
    let ahead_food = look_food();
    if (ahead_food > 0) {
        move();
    } else {
        // Turn randomly (alternate left/right based on energy parity).
        if (my_energy() % 2 == 0) {
            turn_left();
        } else {
            turn_right();
        }
    }

    // Reproduce when wealthy.
    if (my_energy() > 150 && my_hydration() > 50) {
        reproduce();
    }
}
```

### 4. Stateful explorer (uses persistent locals)

```
fn main() {
    let steps = steps;  // persistent: locals survive across ticks
    steps = steps + 1;

    // Change direction every 10 ticks.
    if (steps % 10 == 0) {
        turn_right();
    }

    move();
    eat();
    drink();
}
```

Note: `let steps = steps;` is valid because uninitialized locals default to 0 on first tick and persist thereafter. The compiler may provide `persist` syntax sugar in v2 to make this clearer.

## Compilation mapping

Each language construct maps to bytecode:

| Language | Bytecode |
|----------|----------|
| `let x = expr;` | expr → STORE_LOCAL x_index |
| `x = expr;` | expr → STORE_LOCAL x_index |
| `if (c) { ... }` | c → JMP_IF_FALSE end, ..., end: |
| `if (c) { ... } else { ... }` | c → JMP_IF_FALSE else, ..., JMP end, else: ..., end: |
| `while (c) { ... }` | loop: c → JMP_IF_FALSE end, ..., JMP loop, end: |
| `fn f() { ... }` | code at address, RETURN at end |
| `f()` | CALL addr |
| `return expr;` | expr → RETURN |
| `a + b` | a, b → ADD |
| `move()` | MOVE |
| `eat()` | EAT |
| `perceive_food()` | PERCEIVE, SWAP, POP, SWAP, POP, SWAP, POP |
| `true` | PUSH_BOOL 1 |
| `42` | PUSH_INT 42 |
