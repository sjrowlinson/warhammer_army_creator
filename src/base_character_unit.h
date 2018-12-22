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
    double magic_item_budget_;
    // std::tuple<double, std::size_t, ItemCategory, ItemType> magic_item_budget_;
    double faction_item_budget_;
    // std::tuple<double, std::size_t, ItemCategory, ItemType> faction_item_budget_;
    double total_item_budget_;
    // std::pair<double, std::size_t> total_item_budget_;
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
        double mi_budget,
        double fi_budget,
        double ti_budget,
        bool is_unique,
        std::string mount_name=""
    );
    virtual ~base_character_unit() = default;

    double points() const noexcept;
    const std::vector<std::string>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;
    double magic_item_budget() const noexcept;
    double faction_item_budget() const noexcept;
    double total_item_budget() const noexcept;
    bool unique() const noexcept;
    const std::string& mount_name() const noexcept;
};

#endif // !BASE_CHARACTER_UNIT_H
