"""Tests for the compiler CLI."""

from __future__ import annotations

import struct
import subprocess
import sys
from pathlib import Path

from compiler.emit import MAGIC, VERSION


def test_cli_compiles_random_walker(tmp_path: Path) -> None:
    """Test that the CLI compiles a .gol file to a valid .gob."""
    source = tmp_path / "test.gol"
    source.write_text("fn main() { move(); }")
    output = tmp_path / "test.gob"

    result = subprocess.run(
        [sys.executable, "-m", "compiler", str(source), "-o", str(output)],
        cwd=Path(__file__).parent.parent,
        capture_output=True,
        text=True,
    )

    assert result.returncode == 0, f"stderr: {result.stderr}"
    assert output.exists()

    # Verify .gob header.
    data = output.read_bytes()
    assert len(data) >= 16
    magic, version = struct.unpack_from("<IH", data, 0)
    assert magic == MAGIC
    assert version == VERSION


def test_cli_error_on_bad_source(tmp_path: Path) -> None:
    """Test that the CLI reports errors for invalid source."""
    source = tmp_path / "bad.gol"
    source.write_text("fn main() { @ }")

    result = subprocess.run(
        [sys.executable, "-m", "compiler", str(source)],
        cwd=Path(__file__).parent.parent,
        capture_output=True,
        text=True,
    )

    assert result.returncode != 0
    assert "error" in result.stderr.lower()


def test_cli_error_on_missing_file(tmp_path: Path) -> None:
    result = subprocess.run(
        [sys.executable, "-m", "compiler", str(tmp_path / "nonexistent.gol")],
        cwd=Path(__file__).parent.parent,
        capture_output=True,
        text=True,
    )

    assert result.returncode != 0
