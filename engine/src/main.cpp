#include "engine/version.hpp"

#include <iostream>

int main() {
    std::cout << "tick 0\n";
    std::cout << "engine version " << gol::version() << "\n";
    return 0;
}
