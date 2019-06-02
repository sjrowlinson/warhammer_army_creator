#include "base_mage_character_unit.h"

base_mage_character_unit::base_mage_character_unit(Faction faction,
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
    std::vector<lore_option> &&lores, short lore_count,
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
    std::move(mi_budget),
    std::move(fi_budget),
    std::move(ti_budget),
    is_unique,
    mount_name
), mage_level_(mage_level), lore_count_(lore_count), level_upgrades_(std::move(level_upgrades)),
   lores_(std::move(lores)) { but = BaseUnitType::MAGE_CHARACTER; }

short base_mage_character_unit::mage_level() const noexcept { return mage_level_; }

const std::unordered_map<short, double>& base_mage_character_unit::level_upgrades() const noexcept {
    return level_upgrades_;
}

const std::vector<lore_option> &base_mage_character_unit::lores() const noexcept {
    return lores_;
}

short base_mage_character_unit::lore_count() const noexcept {
    return lore_count_;
}
