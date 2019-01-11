#ifndef BASE_MAGE_CHARACTER_UNIT_H
#define BASE_MAGE_CHARACTER_UNIT_H

#include "enums.h"
#include "base_character_unit.h"

class base_mage_character_unit : public base_character_unit {
private:
    short mage_level_;
    std::unordered_map<short, double> level_upgrades_;
    std::vector<lore_option> lores_;
public:
    explicit base_mage_character_unit(
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
        short mage_level,
        std::unordered_map<short, double>&& level_upgrades,
        std::vector<lore_option>&& lores,
        std::string mount_name=""
    );
    ~base_mage_character_unit() = default;

    short mage_level() const noexcept;
    const std::unordered_map<short, double>& level_upgrades() const noexcept;
    const std::vector<lore_option>& lores() const noexcept;
};

#endif // !BASE_MAGE_CHARACTER_UNIT_H
