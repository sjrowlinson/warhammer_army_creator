#ifndef BASE_MELEE_CHARACTER_UNIT_H
#define BASE_MELEE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_character_unit.h"

class base_melee_character_unit : base_character_unit {
private:
public:
    base_melee_character_unit(
        armies::Faction faction,
        armies::UnitType ut,
        armies::UnitClass uc,
        std::string name,
        double pts,
        std::vector<short>&& stats,
        std::vector<std::string>&& sr,
        equipment&& eq,
        options&& opt,
        double mi_budget,
        double fi_budget,
        double ti_budget,
        bool is_bsb,
        std::string mount_name=""
    );
    ~base_melee_character_unit() = default;
};

#endif // !BASE_MELEE_CHARACTER_UNIT_H