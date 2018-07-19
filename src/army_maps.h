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
        ORCS_AND_GOBLINS,
        NONE
    };

    static std::unordered_map<
        std::string,
        Faction
    > s_map_string_faction = {
        {"The Empire", Faction::EMPIRE},
        {"Bretonnia", Faction::BRETONNIA},
        {"Dwarfs", Faction::DWARFS},
        {"High Elves", Faction::HIGH_ELVES},
        {"Dark Elves", Faction::DARK_ELVES},
        {"Warriors of Chaos", Faction::WARRIORS_OF_CHAOS},
        {"Daemons of Chaos", Faction::DAEMONS_OF_CHAOS},
        {"Beastmen", Faction::BEASTMEN},
        {"Chaos Dwarfs", Faction::CHAOS_DWARFS},
        {"Vampire Counts", Faction::VAMPIRE_COUNTS},
        {"Tomb Kings", Faction::TOMB_KINGS},
        {"Ogre Kingdoms", Faction::OGRE_KINGDOMS},
        {"Skaven", Faction::SKAVEN},
        {"Orcs and Goblins", Faction::ORCS_AND_GOBLINS}
    };

    enum class UnitType {
        LORD,
        HERO,
        CORE,
        SPECIAL,
        RARE,
        NONE
    };

    static std::unordered_map<
        std::string,
        UnitType
    > s_map_string_unit_type = {
        {"Lord", UnitType::LORD},
        {"Hero", UnitType::HERO},
        {"Core", UnitType::CORE},
        {"Special", UnitType::SPECIAL},
        {"Rare", UnitType::RARE}
    };

    enum class UnitClass {
        INFANTRY,
        CAVALRY,
        MONSTROUS_CAVALRY,
        MONSTROUS_CREATURES,
        WARBEASTS,
        CHARIOT,
        MONSTER,
        SWARM,
        WARMACHINE,
        UNIQUE
    };

    static std::unordered_map<
        std::string,
        UnitClass
    > s_map_string_unit_class = {
        {"Infantry", UnitClass::INFANTRY},
        {"Cavalry", UnitClass::CAVALRY},
        {"Monstrous Cavalry", UnitClass::MONSTROUS_CAVALRY},
        {"Monstrous Creatures", UnitClass::MONSTROUS_CREATURES},
        {"Warbeasts", UnitClass::WARBEASTS},
        {"Chariot", UnitClass::CHARIOT},
        {"Monster", UnitClass::MONSTER},
        {"Swarm", UnitClass::SWARM},
        {"Warmachine", UnitClass::WARMACHINE},
        {"Unique", UnitClass::UNIQUE}
    };

}

#endif // !ARMY_MAPS_H
