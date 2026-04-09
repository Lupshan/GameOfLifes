# Bytecode file format — v1

## Overview

A bytecode file (`.gol` extension) contains compiled bot code ready for the engine VM. The format is binary, little-endian, and designed for simplicity over compactness.

## Layout

```
┌──────────────────────┐
│ Header (16 bytes)    │
├──────────────────────┤
│ Constant pool        │
├──────────────────────┤
│ Code section         │
├──────────────────────┤
│ Debug info (optional)│
└──────────────────────┘
```

## Header (16 bytes)

| Offset | Size | Field           | Description                                       |
|--------|------|-----------------|---------------------------------------------------|
| 0      | 4    | magic           | `0x474F4C42` ("GOLB" in ASCII, for **G**ame **O**f **L**ifes **B**ytecode). |
| 4      | 2    | version         | Bytecode format version. Currently `1`.           |
| 6      | 2    | const_count     | Number of entries in the constant pool.            |
| 8      | 4    | code_size       | Size of the code section in bytes.                 |
| 12     | 2    | local_count     | Number of local variable slots needed.             |
| 14     | 2    | flags           | Bit flags (reserved, must be 0 in v1).             |

## Constant pool

`const_count` entries, each prefixed by a 1-byte type tag:

| Tag  | Type   | Encoding                                  |
|------|--------|-------------------------------------------|
| 0x01 | int32  | 4 bytes, little-endian signed.            |
| 0x02 | string | 2-byte length (u16) + UTF-8 bytes.        |

Strings in the constant pool are used for debug names (function names, variable names). They are not referenced by the code section in v1 — only by the debug info section.

## Code section

`code_size` bytes of raw bytecode instructions. See `opcodes.md` for the instruction set.

The entry point is byte 0 of the code section. Execution starts there each tick.

## Debug info (optional)

If present, follows immediately after the code section. Structure:

| Field          | Size   | Description                                    |
|----------------|--------|------------------------------------------------|
| debug_magic    | 4      | `0x44424700` ("DBG\0").                        |
| line_map_count | 2      | Number of entries in the line number map.       |
| line_map       | 4 each | Pairs of (code_offset: u16, source_line: u16). |

The debug info is **stripped** in production builds. The VM never reads it — it's for tooling (debugger, error messages).

## Versioning

The `version` field in the header allows forward-incompatible changes. The VM must reject files with `version > supported_version`. A version bump requires a new ADR.

## Constraints

- Maximum code section size: 65535 bytes (u16 addressing for CALL).
- Maximum constant pool entries: 65535.
- Maximum local variables: 255 (u8 index in LOAD_LOCAL/STORE_LOCAL).
- Maximum stack depth: 256 (runtime limit, not file-level).

## Example

A minimal bot that moves forward every tick:

```
Header: GOLB v1, 0 consts, 2 bytes code, 0 locals
Code: [0x82, 0x00]  // MOVE, HALT
```
