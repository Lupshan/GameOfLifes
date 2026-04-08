#ifndef GAMEOFLIFES_ENGINE_GRID_HPP
#define GAMEOFLIFES_ENGINE_GRID_HPP

#include "engine/position.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace gol {

// Toroidal wrap helper. Handles negative values via the
// double-modulo trick. `size` must be > 0.
inline int wrap(int v, int size) {
    if (size <= 0) {
        throw std::invalid_argument("wrap: size must be positive");
    }
    int r = v % size;
    if (r < 0) {
        r += size;
    }
    return r;
}

// Templated 2D storage with toroidal access. Header-only — genuine
// template requirement (exception to the no-impl-in-header rule).
template <typename T> class Grid {
  public:
    Grid(int width, int height) : width_(width), height_(height) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Grid: dimensions must be positive");
        }
        cells_.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    }

    Grid(int width, int height, const T& fill) : Grid(width, height) {
        for (auto& cell : cells_) {
            cell = fill;
        }
    }

    int width() const noexcept {
        return width_;
    }
    int height() const noexcept {
        return height_;
    }

    const T& at(Position p) const {
        return cells_[index(p)];
    }
    T& at(Position p) {
        return cells_[index(p)];
    }

    void set(Position p, const T& value) {
        cells_[index(p)] = value;
    }

    typename std::vector<T>::iterator begin() noexcept {
        return cells_.begin();
    }
    typename std::vector<T>::iterator end() noexcept {
        return cells_.end();
    }
    typename std::vector<T>::const_iterator begin() const noexcept {
        return cells_.begin();
    }
    typename std::vector<T>::const_iterator end() const noexcept {
        return cells_.end();
    }

  private:
    std::size_t index(Position p) const {
        int x = wrap(p.x, width_);
        int y = wrap(p.y, height_);
        return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
               static_cast<std::size_t>(x);
    }

    int width_;
    int height_;
    std::vector<T> cells_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_GRID_HPP
