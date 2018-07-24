#include "base_character_unit.h"

base_character_unit::base_character_unit(
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
   total_item_budget_(ti_budget), is_bsb_(is_bsb), mount_name_(mount_name) {}

double base_character_unit::points() const noexcept { return pts_; }
std::vector<short> base_character_unit::statistics() const noexcept { return stats_; }
std::vector<std::string> base_character_unit::special_rules() const noexcept { return sr_; }
equipment base_character_unit::eq() const noexcept { return eq_; }
options base_character_unit::opt() const noexcept { return opt_; }
double base_character_unit::magic_item_budget() const noexcept { return magic_item_budget_; }
double base_character_unit::faction_item_budget() const noexcept { return faction_item_budget_; }
double base_character_unit::total_item_budget() const noexcept { return total_item_budget_; }
bool base_character_unit::bsb() const noexcept { return is_bsb_; }
std::string base_character_unit::mount_name() const noexcept { return mount_name_; }
