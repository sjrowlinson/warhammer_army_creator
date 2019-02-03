#ifndef BASE_MIXED_UNIT_H
#define BASE_MIXED_UNIT_H

#include "base_unit.h"
#include "base_normal_unit.h"
#include "equipment.h"
#include "options.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_mixed_unit : public base_unit {
private:
    // COMBINED
    double ratio_limit_; // < defines max. ratio of master size to slave size
    double magic_banner_budget_; // < for master CommandGroup::STANDARD_BEARER only

    // MASTER
    std::string master_name_;
    double master_pts_per_model_;
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > master_opt_command_;
    // => non-champion
    std::vector<std::string> master_stats_;
    std::vector<std::string> master_sr_; // special rules
    equipment master_eq_;
    options master_opt_;
    // => champion
    std::vector<std::string> master_champ_stats_;
    std::vector<std::string> master_champ_sr_; // special rules
    equipment master_champ_eq_;
    options master_champ_opt_;

    // SLAVE
    std::string slave_name_;
    double slave_pts_per_model_;
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > slave_opt_command_;
    // => non-champion
    std::vector<std::string> slave_stats_;
    std::vector<std::string> slave_sr_; // special rules
    equipment slave_eq_;
    options slave_opt_;
    // => champion
    std::vector<std::string> slave_champ_stats_;
    std::vector<std::string> slave_champ_sr_; // special rules
    equipment slave_champ_eq_;
    options slave_champ_opt_;
public:
    explicit base_mixed_unit(
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
    );
    ~base_mixed_unit() = default;

    // COMBINED
    double ratio_limit() const noexcept;
    double magic_banner_budget() const noexcept;

    // MASTER
    const std::string& master_name() const noexcept;
    double master_points_per_model() const noexcept;
    const std::vector<std::string>& master_statistics() const noexcept;
    const std::vector<std::string>& master_special_rules() const noexcept;
    const equipment& master_eq() const noexcept;
    const options& master_opt() const noexcept;
    const std::vector<std::string>& master_champion_statistics() const noexcept;
    const std::vector<std::string>& master_champion_special_rules() const noexcept;
    const equipment& master_champion_eq() const noexcept;
    const options& master_champion_opt() const noexcept;
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& master_optional_command() const noexcept;

    // SLAVE
    const std::string& slave_name() const noexcept;
    double slave_points_per_model() const noexcept;
    const std::vector<std::string>& slave_statistics() const noexcept;
    const std::vector<std::string>& slave_special_rules() const noexcept;
    const equipment& slave_eq() const noexcept;
    const options& slave_opt() const noexcept;
    const std::vector<std::string>& slave_champion_statistics() const noexcept;
    const std::vector<std::string>& slave_champion_special_rules() const noexcept;
    const equipment& slave_champion_eq() const noexcept;
    const options& slave_champion_opt() const noexcept;
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& slave_optional_command() const noexcept;
};

#endif // !BASE_MIXED_UNIT_H
