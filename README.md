# GameOfLifes

[![CI](https://github.com/Lupshan/GameOfLifes/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/Lupshan/GameOfLifes/actions/workflows/ci.yml)


A web platform where users submit small AIs that drive agents in a shared,
persistent simulated world. Inspired by Zappy crossed with Conway's Game of
Life — no winners, no losers, just a rich ecosystem to watch evolve, with a
strong focus on genetics and emergent behavior.

## Quick start

```sh
just            # list available recipes
just fmt        # format C++ and Python
just lint       # lint Python
just build      # build the C++ engine (requires CMakeLists, see step 03)
just test       # run all tests
```

See [CLAUDE.md](CLAUDE.md) for project rules and conventions, and
[docs/roadmap/](docs/roadmap/) for the ordered build plan.
