#ifndef BASE_NORMAL_UNIT_H
#define BASE_NORMAL_UNIT_H

#include "base_unit.h"
#include "../enums.h"
#include "../equipment.h"
#include "../options.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_normal_unit : public base_unit {
private:
    double pts_per_model_;
    // non-champion fields
    std::vector<std::string> stats_;
    std::vector<std::string> sr_; // special rules
    equipment eq_;
    options opt_;

    // champion fields
    std::vector<std::string> champ_stats_;
    std::vector<std::string> champ_sr_; // champion special rules
    equipment champ_eq_;
    options champ_opt_;

    // command group
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > opt_command_;

    // mount name if the unit is cavalry
    std::string mount_name_;
public:
    explicit base_normal_unit(Faction faction,
        UnitType ut,
        UnitCategory uc,
        const std::string& name,
        bool contributes,
        std::size_t min_size,
        std::size_t max_size,
        double pts_per_model,
        std::vector<std::string>&& stats,
        std::vector<std::string>&& sr,
        equipment&& eq,
        options&& opt,
        std::vector<std::string>&& champ_stats,
        std::vector<std::string>&& champ_sr,
        equipment&& champ_eq,
        options&& champ_opt,
        const budget& champ_mi_budget,
        const budget& champ_fi_budget,
        const budget& champ_ti_budget,
        std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        >&& opt_command,
        double magic_banner_budget,
        std::string mount_name=""
    );
    ~base_normal_unit() = default;

    double points_per_model() const noexcept;

    const std::vector<std::string>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;

    const std::vector<std::string>& champion_statistics() const noexcept;
    const std::vector<std::string>& champion_special_rules() const noexcept;
    const equipment& champion_eq() const noexcept;
    const options& champion_opt() const noexcept;

    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& optional_command() const noexcept;

    const std::string& mount_name() const noexcept;
};

#endif // !BASE_NORMAL_UNIT_H
