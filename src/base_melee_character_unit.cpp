#include "base_melee_character_unit.h"

base_melee_character_unit::base_melee_character_unit(
    Faction faction,
    UnitType ut,
    UnitClass uc,
    const std::string& name,
    double pts,
    std::vector<std::string>&& stats,
    std::vector<std::string>&& sr,
    equipment&& eq,
    options&& opt,
    double mi_budget,
    double fi_budget,
    double ti_budget,
    bool is_unique,
    std::string mount_name
) : base_character_unit(
    faction,
    ut,
    uc,
    name,
    pts,
    std::move(stats),
    std::move(sr),
    std::move(eq),
    std::move(opt),
    mi_budget,
    fi_budget,
    ti_budget,
    is_unique,
    mount_name
) { but = BaseUnitType::MELEE_CHARACTER; }
