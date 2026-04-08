#include <iostream>

#include "engine/version.hpp"

int main()
{
    std::cout << "tick 0\n";
    std::cout << "engine version " << gol::version() << "\n";
    return 0;
}
