#include "engine/broadcast.hpp"

namespace gol {

RadioGrid::RadioGrid(int width, int height) : grid_(width, height, 0) {
}

void RadioGrid::broadcast(Position pos, std::int32_t message, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            Position target{pos.x + dx, pos.y + dy};
            grid_.set(target, message);
        }
    }
}

std::int32_t RadioGrid::listen(Position pos) const {
    return grid_.at(pos);
}

void RadioGrid::reset() {
    for (auto& cell : grid_) {
        cell = 0;
    }
}

} // namespace gol
