#include "base_character_unit.h"

base_character_unit::base_character_unit(Faction faction,
    UnitType ut,
    UnitCategory uc,
    const std::string& name,
    double pts,
    std::vector<std::string>&& stats,
    std::vector<std::string>&& sr,
    equipment&& eq,
    options&& opt,
    const budget &mi_budget,
    const budget &fi_budget,
    const budget &ti_budget,
    bool is_unique,
    std::string mount_name
) : base_unit(
    faction,
    ut,
    uc,
    name,
    mi_budget,
    fi_budget,
    ti_budget,
    1U,
    1U
), pts_(pts), stats_(std::move(stats)), sr_(std::move(sr)),
   eq_(std::move(eq)), opt_(std::move(opt)),
   is_unique(is_unique), mount_name_(mount_name) {}

double base_character_unit::points() const noexcept { return pts_; }
const std::vector<std::string>& base_character_unit::statistics() const noexcept { return stats_; }
const std::vector<std::string>& base_character_unit::special_rules() const noexcept { return sr_; }
const equipment& base_character_unit::eq() const noexcept { return eq_; }
const options& base_character_unit::opt() const noexcept { return opt_; }
bool base_character_unit::unique() const noexcept { return is_unique; }
const std::string& base_character_unit::mount_name() const noexcept { return mount_name_; }
