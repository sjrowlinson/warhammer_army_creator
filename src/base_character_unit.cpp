#include "base_character_unit.h"

base_character_unit::base_character_unit(
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
) : base_unit(
    faction,
    ut,
    uc,
    name,
    1U,
    1U
), pts_(pts), stats_(std::move(stats)), sr_(std::move(sr)),
   eq_(std::move(eq)), opt_(std::move(opt)),
   magic_item_budget_(mi_budget), faction_item_budget_(fi_budget),
   total_item_budget_(ti_budget), is_unique(is_unique), mount_name_(mount_name) {}

double base_character_unit::points() const noexcept { return pts_; }
const std::vector<std::string>& base_character_unit::statistics() const noexcept { return stats_; }
const std::vector<std::string>& base_character_unit::special_rules() const noexcept { return sr_; }
const equipment& base_character_unit::eq() const noexcept { return eq_; }
const options& base_character_unit::opt() const noexcept { return opt_; }
double base_character_unit::magic_item_budget() const noexcept { return magic_item_budget_; }
double base_character_unit::faction_item_budget() const noexcept { return faction_item_budget_; }
double base_character_unit::total_item_budget() const noexcept { return total_item_budget_; }
bool base_character_unit::unique() const noexcept { return is_unique; }
const std::string& base_character_unit::mount_name() const noexcept { return mount_name_; }
