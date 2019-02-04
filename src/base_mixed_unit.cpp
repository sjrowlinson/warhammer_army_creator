#include "base_mixed_unit.h"

base_mixed_unit::base_mixed_unit(
    Faction faction,
    UnitType ut,
    UnitCategory uc,
    const std::string& name,
    const std::string& master_name,
    std::size_t master_min_size,
    std::size_t master_max_size,
    double master_pts_per_model,
    std::vector<std::string>&& master_stats,
    std::vector<std::string>&& master_sr,
    equipment&& master_eq,
    options&& master_opt,
    std::vector<std::string>&& master_champ_stats,
    std::vector<std::string>&& master_champ_sr,
    equipment&& master_champ_eq,
    options&& master_champ_opt,
    const budget& master_champ_mi_budget,
    const budget& master_champ_fi_budget,
    const budget& master_champ_ti_budget,
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >&& master_opt_command,
    const std::string& slave_name,
    std::size_t slave_min_size,
    std::size_t slave_max_size,
    double slave_pts_per_model,
    std::vector<std::string>&& slave_stats,
    std::vector<std::string>&& slave_sr,
    equipment&& slave_eq,
    options&& slave_opt,
    std::vector<std::string>&& slave_champ_stats,
    std::vector<std::string>&& slave_champ_sr,
    equipment&& slave_champ_eq,
    options&& slave_champ_opt,
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >&& slave_opt_command,
    double magic_banner_budget
) : base_unit(
    faction,
    ut,
    uc,
    name,
    master_champ_mi_budget,
    master_champ_fi_budget,
    master_champ_ti_budget,
    master_min_size,
    master_max_size
), ratio_limit_(static_cast<double>(master_min_size)/slave_min_size),
   magic_banner_budget_(magic_banner_budget),
   master_name_(master_name),
   master_min_size_(master_min_size),
   master_max_size_(master_max_size),
   master_pts_per_model_(master_pts_per_model),
   master_opt_command_(std::move(master_opt_command)),
   master_stats_(std::move(master_stats)),
   master_sr_(std::move(master_sr)),
   master_eq_(std::move(master_eq)), master_opt_(std::move(master_opt)),
   master_champ_stats_(std::move(master_champ_stats)),
   master_champ_sr_(std::move(master_champ_sr)),
   master_champ_eq_(std::move(master_champ_eq)), master_champ_opt_(std::move(master_champ_opt)),
   slave_name_(slave_name),
   slave_min_size_(slave_min_size),
   slave_max_size_(slave_max_size),
   slave_pts_per_model_(slave_pts_per_model),
   slave_opt_command_(std::move(slave_opt_command)),
   slave_stats_(std::move(slave_stats)),
   slave_sr_(std::move(slave_sr)),
   slave_eq_(std::move(slave_eq)), slave_opt_(std::move(slave_opt)),
   slave_champ_stats_(std::move(slave_champ_stats)),
   slave_champ_sr_(std::move(slave_champ_sr)),
   slave_champ_eq_(std::move(slave_champ_eq)), slave_champ_opt_(std::move(slave_champ_opt)) {
    but = BaseUnitType::MIXED;
}

double base_mixed_unit::ratio_limit() const noexcept {
    return ratio_limit_;
}

double base_mixed_unit::magic_banner_budget() const noexcept {
    return magic_banner_budget_;
}

// MASTER
const std::string& base_mixed_unit::master_name() const noexcept {
    return master_name_;
}
std::size_t base_mixed_unit::master_min_size() const noexcept {
    return master_min_size_;
}
std::size_t base_mixed_unit::master_max_size() const noexcept {
    return master_max_size_;
}
double base_mixed_unit::master_points_per_model() const noexcept {
    return master_pts_per_model_;
}
const std::vector<std::string>& base_mixed_unit::master_statistics() const noexcept {
    return master_stats_;
}
const std::vector<std::string>& base_mixed_unit::master_special_rules() const noexcept {
    return master_sr_;
}
const equipment& base_mixed_unit::master_eq() const noexcept {
    return master_eq_;
}
const options& base_mixed_unit::master_opt() const noexcept {
    return master_opt_;
}
const std::vector<std::string>&base_mixed_unit:: master_champion_statistics() const noexcept {
    return master_champ_stats_;
}
const std::vector<std::string>&base_mixed_unit:: master_champion_special_rules() const noexcept {
    return master_champ_sr_;
}
const equipment& base_mixed_unit::master_champion_eq() const noexcept {
    return master_champ_eq_;
}
const options& base_mixed_unit::master_champion_opt() const noexcept {
    return master_champ_opt_;
}
const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& base_mixed_unit::master_optional_command() const noexcept {
    return master_opt_command_;
}

// SLAVE
const std::string& base_mixed_unit::slave_name() const noexcept {
    return slave_name_;
}
std::size_t base_mixed_unit::slave_min_size() const noexcept {
    return slave_min_size_;
}
std::size_t base_mixed_unit::slave_max_size() const noexcept {
    return slave_max_size_;
}
double base_mixed_unit::slave_points_per_model() const noexcept {
    return slave_pts_per_model_;
}
const std::vector<std::string>& base_mixed_unit::slave_statistics() const noexcept {
    return slave_stats_;
}
const std::vector<std::string>& base_mixed_unit::slave_special_rules() const noexcept {
    return slave_sr_;
}
const equipment& base_mixed_unit::slave_eq() const noexcept {
    return slave_eq_;
}
const options& base_mixed_unit::slave_opt() const noexcept {
    return slave_opt_;
}
const std::vector<std::string>&base_mixed_unit:: slave_champion_statistics() const noexcept {
    return slave_champ_stats_;
}
const std::vector<std::string>&base_mixed_unit:: slave_champion_special_rules() const noexcept {
    return slave_champ_sr_;
}
const equipment& base_mixed_unit::slave_champion_eq() const noexcept {
    return slave_champ_eq_;
}
const options& base_mixed_unit::slave_champion_opt() const noexcept {
    return slave_champ_opt_;
}
const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& base_mixed_unit::slave_optional_command() const noexcept {
    return slave_opt_command_;
}
