#ifndef BASE_MELEE_CHARACTER_UNIT_H
#define BASE_MELEE_CHARACTER_UNIT_H

#include "enums.h"
#include "base_character_unit.h"

class base_melee_character_unit : public base_character_unit {
private:
public:
    explicit base_melee_character_unit(
        Faction faction,
        UnitType ut,
        UnitCategory uc,
        const std::string& name,
        double pts,
        std::vector<std::string>&& stats,
        std::vector<std::string>&& sr,
        equipment&& eq,
        options&& opt,
        budget&& mi_budget,
        budget&& fi_budget,
        budget&& ti_budget,
        bool is_unique,
        std::string mount_name=""
    );
    ~base_melee_character_unit() = default;
};

#endif // !BASE_MELEE_CHARACTER_UNIT_H
