#ifndef GAMEOFLIFES_ENGINE_DIRECTION_HPP
#define GAMEOFLIFES_ENGINE_DIRECTION_HPP

#include "engine/position.hpp"

namespace gol {

enum class Direction { North, East, South, West };

Position to_offset(Direction d) noexcept;
Direction turn_left(Direction d) noexcept;
Direction turn_right(Direction d) noexcept;

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_DIRECTION_HPP
