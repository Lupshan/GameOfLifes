#include "engine/agent_vm.hpp"

#include "engine/direction.hpp"
#include "engine/genome.hpp"
#include "engine/terrain.hpp"
#include "engine/traits.hpp"

#include <algorithm>

namespace gol {

AgentIntrinsicHandler::AgentIntrinsicHandler(std::size_t agent_index, World& world)
    : agent_index_(agent_index), world_(world) {
}

void AgentIntrinsicHandler::set_agent_index(std::size_t index) {
    agent_index_ = index;
}

Agent& AgentIntrinsicHandler::agent() {
    if (agent_index_ >= world_.agents().size()) {
        // Index became stale (e.g., agents reaped mid-tick). Fall back to first alive agent.
        // This should not happen in normal flow but prevents UB.
        agent_index_ = 0;
    }
    return world_.agents()[agent_index_];
}

VmStatus AgentIntrinsicHandler::handle_perceive(Vm& vm) {
    Agent& a = agent();
    Position p = a.pos;
    int terrain_val = static_cast<int>(world_.terrain().at(p));
    int food_val = world_.food().at(p);
    int water_val = world_.water().at(p);

    int agents_here = 0;
    for (const auto& other : world_.agents()) {
        if (other.alive && other.pos == p) {
            ++agents_here;
        }
    }

    vm.push(terrain_val);
    vm.push(food_val);
    vm.push(water_val);
    vm.push(agents_here);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_look(Vm& vm) {
    Agent& a = agent();
    Position offset = to_offset(a.facing);
    Position target = world_.wrap_position(Position{a.pos.x + offset.x, a.pos.y + offset.y});

    int terrain_val = static_cast<int>(world_.terrain().at(target));
    int food_val = world_.food().at(target);
    int water_val = world_.water().at(target);

    int agents_there = 0;
    for (const auto& other : world_.agents()) {
        if (other.alive && other.pos == target) {
            ++agents_there;
        }
    }

    vm.push(terrain_val);
    vm.push(food_val);
    vm.push(water_val);
    vm.push(agents_there);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_move(Vm& vm) {
    Agent& a = agent();
    Position offset = to_offset(a.facing);
    Position target = world_.wrap_position(Position{a.pos.x + offset.x, a.pos.y + offset.y});

    Terrain t = world_.terrain().at(target);
    Traits traits = decode(a.genome);
    int cost = movement_cost(t);
    if (t == Terrain::Water && traits.swim > 0) {
        cost = std::max(1, cost - traits.swim);
    }
    a.energy -= (cost - 1);
    a.pos = target;

    (void)vm;
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_turn_left(Vm& vm) {
    agent().facing = turn_left(agent().facing);
    (void)vm;
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_turn_right(Vm& vm) {
    agent().facing = turn_right(agent().facing);
    (void)vm;
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_eat(Vm& vm) {
    Agent& a = agent();
    std::uint8_t& food_cell = world_.food().at(a.pos);
    Traits traits = decode(a.genome);
    int gained = 0;
    if (food_cell > 0) {
        gained = food_cell * traits.forage_efficiency;
        a.energy += gained;
        food_cell = 0;
    }
    vm.push(gained);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_drink(Vm& vm) {
    Agent& a = agent();
    std::uint8_t& water_cell = world_.water().at(a.pos);
    Traits traits = decode(a.genome);
    int gained = 0;
    if (water_cell > 0) {
        gained = water_cell * traits.forage_efficiency;
        a.hydration += gained;
        water_cell = 0;
    }
    vm.push(gained);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_reproduce(Vm& vm) {
    Agent& a = agent();
    Traits traits = decode(a.genome);
    if (a.energy < traits.reproduction_threshold) {
        vm.push(0);
        return VmStatus::Ok;
    }

    int dir_index = world_.rng().uniform_int(0, 3);
    auto dir = static_cast<Direction>(dir_index);
    Position offset = to_offset(dir);
    Position child_pos = world_.wrap_position(Position{a.pos.x + offset.x, a.pos.y + offset.y});

    int child_energy = a.energy / 2;
    a.energy -= child_energy;

    Genome child_genome = mutate(a.genome, world_.rng(), world_.config().mutation_rate);
    world_.spawn_child(child_pos, child_energy, child_genome, a.id, a.generation + 1);

    vm.push(1);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_my_energy(Vm& vm) {
    vm.push(agent().energy);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_my_hydration(Vm& vm) {
    vm.push(agent().hydration);
    return VmStatus::Ok;
}

VmStatus AgentIntrinsicHandler::handle_my_trait(Vm& vm, std::uint8_t trait_id) {
    Traits traits = decode(agent().genome);
    int value = 0;
    switch (trait_id) {
    case 0:
        value = traits.vision;
        break;
    case 1:
        value = traits.speed;
        break;
    case 2:
        value = traits.metabolism;
        break;
    case 3:
        value = traits.reproduction_threshold;
        break;
    case 4:
        value = traits.swim;
        break;
    case 5:
        value = traits.forage_efficiency;
        break;
    default:
        value = 0;
        break;
    }
    vm.push(value);
    return VmStatus::Ok;
}

} // namespace gol
