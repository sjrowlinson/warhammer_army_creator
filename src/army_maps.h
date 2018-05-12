#include <array>
#include <cstddef>
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

    struct unit_properties {
        std::size_t pts_per_model;
        std::size_t min_size;
        std::array<short, 9U> stat_table;
        std::vector<std::pair<std::string, std::string>> equipment;
        std::vector<std::string> special_rules;
        std::vector<
            std::tuple<std::string, std::string, double>
        > options;
    };

    std::unordered_map<std::string, unit_properties> SKAVEN_ARMY = {
        {"Warlord", unit_properties{
            90U,
            0U,
            {5, 6, 4, 4, 4, 3, 7, 4, 7},
            {
                {"Hand Weapon", ""},
                {"Heavy Armour", ""}
            },
            {"Scurry Away!", "Strength in Numbers", "Verminous Valour"},
            {
                {"Great weapon", "", 6.0},
                {"Additional hand weapon", "", 3.0},
                {"Halberd", "", 3.0},
                {"Shield", "", 3.0},
                {"Rat Ogre Bonebreaker", "", 65.0},
                {"War-litter", "", 35.0},
                {"Great Pox Rat", "", 30.0}
            }
        }},
        {"Grey Seer", unit_properties{
            240U,
            0U,
            {5, 3, 3, 3, 4, 3, 5, 1, 7},
            {},
            {},
            {}
        }}
    };
};
