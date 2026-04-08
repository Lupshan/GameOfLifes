#include "engine/direction.hpp"

namespace gol {

Position to_offset(Direction d) noexcept {
    switch (d) {
    case Direction::North:
        return {0, -1};
    case Direction::East:
        return {1, 0};
    case Direction::South:
        return {0, 1};
    case Direction::West:
        return {-1, 0};
    }
    return {0, 0};
}

Direction turn_left(Direction d) noexcept {
    switch (d) {
    case Direction::North:
        return Direction::West;
    case Direction::West:
        return Direction::South;
    case Direction::South:
        return Direction::East;
    case Direction::East:
        return Direction::North;
    }
    return d;
}

Direction turn_right(Direction d) noexcept {
    switch (d) {
    case Direction::North:
        return Direction::East;
    case Direction::East:
        return Direction::South;
    case Direction::South:
        return Direction::West;
    case Direction::West:
        return Direction::North;
    }
    return d;
}

} // namespace gol
