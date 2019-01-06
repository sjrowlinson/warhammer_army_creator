#ifndef BASE_CHARACTER_UNIT_H
#define BASE_CHARACTER_UNIT_H

#include "base_unit.h"
#include "enums.h"
#include "equipment.h"
#include "options.h"

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

class base_character_unit : public base_unit {
private:
    double pts_;
    std::vector<std::string> stats_;
    std::vector<std::string> sr_;
    equipment eq_;
    options opt_;
    bool is_unique;

    std::string mount_name_;
public:
    explicit base_character_unit(
        Faction faction,
        UnitType ut,
        UnitCategory uc,
        const std::string& name,
        double pts,
        std::vector<std::string>&& stats,
        std::vector<std::string>&& sr,
        equipment&& eq,
        options&& opt,
        const budget& mi_budget,
        const budget& fi_budget,
        const budget& ti_budget,
        bool is_unique,
        std::string mount_name=""
    );
    virtual ~base_character_unit() = default;

    double points() const noexcept;
    const std::vector<std::string>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;
    bool unique() const noexcept;
    const std::string& mount_name() const noexcept;
};

#endif // !BASE_CHARACTER_UNIT_H
