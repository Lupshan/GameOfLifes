#ifndef GAMEOFLIFES_ENGINE_BROADCAST_HPP
#define GAMEOFLIFES_ENGINE_BROADCAST_HPP

#include "engine/grid.hpp"
#include "engine/position.hpp"

#include <cstdint>

namespace gol {

// Ephemeral radio grid: agents broadcast messages, others listen.
// Reset every tick. Each cell holds the last broadcast value (0 = no message).
class RadioGrid {
  public:
    RadioGrid(int width, int height);

    void broadcast(Position pos, std::int32_t message, int radius);
    std::int32_t listen(Position pos) const;
    void reset();

  private:
    Grid<std::int32_t> grid_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BROADCAST_HPP
