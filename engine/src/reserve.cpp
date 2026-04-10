#include "engine/reserve.hpp"

namespace gol {

bool inside_reserve(const Reserve& r, Position p) {
    return p.x >= r.origin.x && p.x < r.origin.x + r.width && p.y >= r.origin.y &&
           p.y < r.origin.y + r.height;
}

} // namespace gol
