#ifndef ARMY_MAPS_H
#define ARMY_MAPS_H
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace armies {
    
    enum class Faction {
        EMPIRE,
        BRETONNIA,
        DWARFS,
        HIGH_ELVES,
        WOOD_ELVES,
        DARK_ELVES,
        WARRIORS_OF_CHAOS,
        DAEMONS_OF_CHAOS,
        BEASTMEN,
        CHAOS_DWARFS,
        VAMPIRE_COUNTS,
        TOMB_KINGS,
        OGRE_KINGDOMS,
        SKAVEN,
        ORCS_AND_GOBLINS
    };

    enum class UnitType {
        LORD,
        HERO,
        CORE,
        SPECIAL,
        RARE
    };

    struct unit_properties {
        UnitType unit_type;
        std::size_t pts_per_model;
        std::size_t min_size;
        std::array<short, 9U> stat_table;
        std::vector<std::pair<std::string, std::string>> equipment;
        std::vector<std::string> special_rules;
        std::vector<
            std::tuple<std::string, std::string, double>
        > options;
    };

};

#endif // !ARMY_MAPS_H
