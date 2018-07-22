#ifndef BASE_MAGE_CHARACTER_UNIT_H
#define BASE_MAGE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_character_unit.h"

class base_mage_character_unit : public base_character_unit {
private:
    short mage_level_;
    std::unordered_map<short, double> level_upgrades_;
    std::vector<std::string> lores_;
public:
    base_mage_character_unit(
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
        short mage_level,
        std::unordered_map<short, double>&& level_upgrades,
        std::vector<std::string>&& lores,
        std::string mount_name=""
    );
    ~base_mage_character_unit() = default;

    short mage_level() const noexcept;
    const std::unordered_map<short, double>& level_upgrades() const noexcept;
    const std::vector<std::string>& lores() const noexcept;
};

#endif // !BASE_MAGE_CHARACTER_UNIT_H