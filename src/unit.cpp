#include "unit.h"

unit::unit(armies::Faction faction,
           const std::string& name,
           std::size_t points_per_model,
           std::size_t size=0U,
           std::size_t min_size=0U)
    : race(faction),
      name(name),
      points_per_model(points_per_model),
      size(size),
      min_size(min_size) {
    switch(race) {
        case armies::Faction::EMPIRE: break;
        case armies::Faction::BRETONNIA: break;
        case armies::Faction::DWARFS: break;
        case armies::Faction::HIGH_ELVES: break;
        case armies::Faction::WOOD_ELVES: break;
        case armies::Faction::DARK_ELVES: break;
        case armies::Faction::WARRIORS_OF_CHAOS: break;
        case armies::Faction::DAEMONS_OF_CHAOS: break;
        case armies::Faction::BEASTMEN: break;
        case armies::Faction::CHAOS_DWARFS: break;
        case armies::Faction::VAMPIRE_COUNTS: break;
        case armies::Faction::TOMB_KINGS: break;
        case armies::Faction::OGRE_KINGDOMS: break;
        case armies::Faction::SKAVEN:
            army_map = std::make_shared<
                std::unordered_map<std::string, armies::unit_properties>
            >(armies::SKAVEN_ARMY);
        case armies::Faction::ORCS_AND_GOBLINS: break;
    }
}

unit::~unit() {}

std::size_t unit::points_value() const noexcept { 
    return points_per_model * size;
}

const std::string& unit::get_name() const noexcept { return name; }

std::size_t unit::unit_size() const noexcept { return size; }

std::size_t unit::minimum_unit_size() const noexcept { return min_size; }
