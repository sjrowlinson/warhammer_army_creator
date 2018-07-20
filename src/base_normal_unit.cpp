#include "base_normal_unit.h"

base_normal_unit::base_normal_unit(
    armies::Faction faction,
    armies::UnitType ut,
    armies::UnitClass uc,
    std::string name,
    std::size_t min_size,
    std::size_t max_size,
    double pts_per_model,
    std::vector<short>&& stats,
    std::vector<std::string>&& sr,
    equipment&& eq,
    options&& opt,
    std::vector<short>&& champ_stats,
    std::vector<std::string>&& champ_sr,
    equipment&& champ_eq,
    options&& champ_opt,
    double champ_mi_budget,
    double champ_fi_budget,
    double champ_ti_budget,
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >&& opt_command,
    double magic_banner_budget,
    std::string mount_name
) : _base_unit(
    faction,
    ut,
    uc,
    name,
    min_size,
    max_size
), pts_per_model_(pts_per_model), stats_(std::move(stats)), sr_(std::move(sr)), eq_(std::move(eq)),
   opt_(std::move(opt)), champ_stats_(std::move(champ_stats)),
   champ_sr_(std::move(champ_sr)), champ_eq_(std::move(champ_eq)),
   champ_opt_(std::move(champ_opt)), champ_mi_budget_(champ_mi_budget),
   champ_fi_budget_(champ_fi_budget), champ_ti_budget_(champ_ti_budget),
   opt_command_(std::move(opt_command)), magic_banner_budget_(magic_banner_budget),
   mount_name_(mount_name) { but = BaseUnitType::NORMAL; }

double base_normal_unit::points_per_model() const noexcept { return pts_per_model_; }

const std::vector<short>& base_normal_unit::statistics() const noexcept { return stats_; }
const std::vector<std::string>& base_normal_unit::special_rules() const noexcept {
    return sr_;
}
const equipment& base_normal_unit::eq() const noexcept { return eq_; }
const options& base_normal_unit::opt() const noexcept { return opt_; }

const std::vector<short>& base_normal_unit::champion_statistics() const noexcept {
    return champ_stats_;
}
const std::vector<std::string>& base_normal_unit::champion_special_rules() const noexcept {
    return champ_sr_;
}
const equipment& base_normal_unit::champion_eq() const noexcept { return champ_eq_; }
const options& base_normal_unit::champion_opt() const noexcept { return champ_opt_; }
double base_normal_unit::champion_magic_item_budget() const noexcept {
    return champ_mi_budget_;
}
double base_normal_unit::champion_faction_item_budget() const noexcept {
    return champ_fi_budget_;
}
double base_normal_unit::champion_total_item_budget() const noexcept {
    return champ_ti_budget_;
}

const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& base_normal_unit::optional_command() const noexcept {
    return opt_command_;
}
double base_normal_unit::magic_banner_budget() const noexcept { return magic_banner_budget_; }

std::string base_normal_unit::mount_name() const noexcept { return mount_name_; }
