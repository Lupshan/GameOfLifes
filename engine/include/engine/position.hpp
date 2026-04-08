#ifndef GAMEOFLIFES_ENGINE_POSITION_HPP
#define GAMEOFLIFES_ENGINE_POSITION_HPP

#include <cstddef>
#include <functional>

namespace gol {

struct Position {
    int x;
    int y;
};

inline bool operator==(const Position& a, const Position& b) noexcept {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Position& a, const Position& b) noexcept {
    return !(a == b);
}

} // namespace gol

namespace std {

template <> struct hash<gol::Position> {
    std::size_t operator()(const gol::Position& p) const noexcept {
        std::size_t hx = std::hash<int>{}(p.x);
        std::size_t hy = std::hash<int>{}(p.y);
        return hx ^ (hy + 0x9e3779b97f4a7c15ULL + (hx << 6) + (hx >> 2));
    }
};

} // namespace std

#endif // GAMEOFLIFES_ENGINE_POSITION_HPP
