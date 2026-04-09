#include "engine/terrain.hpp"

namespace gol {

int movement_cost(Terrain t) {
    switch (t) {
    case Terrain::Plain:
        return 1;
    case Terrain::Forest:
        return 1;
    case Terrain::Water:
        return 3;
    case Terrain::Rock:
        return 2;
    }
    return 1;
}

double food_regen_factor(Terrain t) {
    switch (t) {
    case Terrain::Plain:
        return 1.0;
    case Terrain::Forest:
        return 3.0; // forests grow food faster
    case Terrain::Water:
        return 0.2; // very little food in water
    case Terrain::Rock:
        return 0.3; // sparse food on rock
    }
    return 1.0;
}

double water_regen_factor(Terrain t) {
    switch (t) {
    case Terrain::Plain:
        return 0.5;
    case Terrain::Forest:
        return 1.0;
    case Terrain::Water:
        return 5.0; // water terrain is full of water
    case Terrain::Rock:
        return 0.1; // very little water on rock
    }
    return 0.5;
}

double mineral_regen_factor(Terrain t) {
    switch (t) {
    case Terrain::Plain:
        return 0.3;
    case Terrain::Forest:
        return 0.5;
    case Terrain::Water:
        return 0.1;
    case Terrain::Rock:
        return 3.0; // rock terrain is rich in minerals
    }
    return 0.3;
}

} // namespace gol
