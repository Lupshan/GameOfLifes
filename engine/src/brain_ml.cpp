#include "engine/brain_ml.hpp"

#include "engine/direction.hpp"
#include "engine/terrain.hpp"
#include "engine/traits.hpp"

#include <algorithm>

namespace gol {

MlBrain::MlBrain(const FeedForwardNet& net) : net_(net) {
}

std::vector<float> MlBrain::encode_perception(const Agent& agent, const World& world) const {
    Position p = agent.pos;
    float terrain_val = static_cast<float>(world.terrain().at(p));
    float food_val = static_cast<float>(world.food().at(p));
    float water_val = static_cast<float>(world.water().at(p));

    int agents_here = 0;
    for (const auto& a : world.agents()) {
        if (a.alive && a.pos == p) {
            ++agents_here;
        }
    }

    Traits traits = decode(agent.genome);

    return {terrain_val,
            food_val,
            water_val,
            static_cast<float>(agents_here),
            static_cast<float>(agent.energy),
            static_cast<float>(agent.hydration),
            static_cast<float>(traits.speed),
            static_cast<float>(traits.metabolism),
            static_cast<float>(traits.swim),
            static_cast<float>(traits.forage_efficiency)};
}

void MlBrain::execute_action(int action, Agent& agent, World& world) const {
    // Actions: 0=move, 1=turn_left, 2=turn_right, 3=eat, 4=drink, 5=reproduce
    switch (action) {
    case 0: {
        // Move forward.
        Position offset = to_offset(agent.facing);
        Position target =
            world.wrap_position(Position{agent.pos.x + offset.x, agent.pos.y + offset.y});
        Terrain t = world.terrain().at(target);
        Traits traits = decode(agent.genome);
        int cost = movement_cost(t);
        if (t == Terrain::Water && traits.swim > 0) {
            cost = std::max(1, cost - traits.swim);
        }
        agent.energy -= (cost - 1);
        agent.pos = target;
        break;
    }
    case 1:
        agent.facing = turn_left(agent.facing);
        break;
    case 2:
        agent.facing = turn_right(agent.facing);
        break;
    case 3: {
        // Eat.
        std::uint8_t& food_cell = world.food().at(agent.pos);
        if (food_cell > 0) {
            Traits traits = decode(agent.genome);
            agent.energy += food_cell * traits.forage_efficiency;
            food_cell = 0;
        }
        break;
    }
    case 4: {
        // Drink.
        std::uint8_t& water_cell = world.water().at(agent.pos);
        if (water_cell > 0) {
            Traits traits = decode(agent.genome);
            agent.hydration += water_cell * traits.forage_efficiency;
            water_cell = 0;
        }
        break;
    }
    case 5: {
        // Reproduce (if able).
        Traits traits = decode(agent.genome);
        if (agent.energy >= traits.reproduction_threshold) {
            int dir_index = world.rng().uniform_int(0, 3);
            auto dir = static_cast<Direction>(dir_index);
            Position offset = to_offset(dir);
            Position child_pos =
                world.wrap_position(Position{agent.pos.x + offset.x, agent.pos.y + offset.y});
            int child_energy = agent.energy / 2;
            agent.energy -= child_energy;
            Genome child_genome = mutate(agent.genome, world.rng(), world.config().mutation_rate);
            world.spawn_child(
                child_pos, child_energy, child_genome, agent.id, agent.generation + 1);
        }
        break;
    }
    default:
        break;
    }
}

void MlBrain::tick(Agent& agent, World& world) {
    std::vector<float> input = encode_perception(agent, world);
    std::vector<float> output = infer(net_, input);
    int action = argmax(output);
    execute_action(action, agent, world);
}

} // namespace gol
