"""Binary .gob file emitter — serializes code + constants to the bytecode spec format."""

from __future__ import annotations

import struct

# Magic: "GOLB" = 0x474F4C42
MAGIC = 0x474F4C42
VERSION = 1


def emit_gob(code: bytes, local_count: int, constants: list[int] | None = None) -> bytes:
    """Produce a .gob binary blob from compiled code."""
    if constants is None:
        constants = []

    # Build constant pool.
    const_pool = bytearray()
    for c in constants:
        const_pool.append(0x01)  # int32 tag
        const_pool.extend(struct.pack("<i", c))

    # Header: 16 bytes.
    header = struct.pack(
        "<IHHIHH",
        MAGIC,
        VERSION,
        len(constants),
        len(code),
        local_count,
        0,  # flags (reserved)
    )

    return header + bytes(const_pool) + code
