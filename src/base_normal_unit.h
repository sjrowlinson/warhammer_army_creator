#ifndef BASE_NORMAL_UNIT_H
#define BASE_NORMAL_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "_base_unit.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_normal_unit : public _base_unit {
private:
    double pts_per_model_;
    // non-champion fields
    std::vector<short> stats_;
    std::vector<std::string> sr_; // special rules
    equipment eq_;
    options opt_;

    // champion fields
    std::vector<short> champ_stats_;
    std::vector<std::string> champ_sr_; // champion special rules
    equipment champ_eq_;
    options champ_opt_;
    double champ_mi_budget_;
    double champ_fi_budget_;
    double champ_ti_budget_;

    // command group
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > opt_command_;
    double magic_banner_budget_;

    // mount name if the unit is cavalry
    std::string mount_name_;
public:
    base_normal_unit(
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
        std::string mount_name=""
    );
    ~base_normal_unit() = default;

    double points_per_model() const noexcept;

    const std::vector<short>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;

    const std::vector<short>& champion_statistics() const noexcept;
    const std::vector<std::string>& champion_special_rules() const noexcept;
    const equipment& champion_eq() const noexcept;
    const options& champion_opt() const noexcept;
    double champion_magic_item_budget() const noexcept;
    double champion_faction_item_budget() const noexcept;
    double champion_total_item_budget() const noexcept;

    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& optional_command() const noexcept;
    double magic_banner_budget() const noexcept;

    std::string mount_name() const noexcept;
};

#endif // !BASE_NORMAL_UNIT_H