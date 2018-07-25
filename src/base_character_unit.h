#ifndef BASE_CHARACTER_UNIT_H
#define BASE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_character_unit : public base_unit {
private:
    double pts_;
    std::vector<short> stats_;
    std::vector<std::string> sr_;
    equipment eq_;
    options opt_;
    double magic_item_budget_;
    double faction_item_budget_;
    double total_item_budget_;
    bool is_bsb_;

    std::string mount_name_;
public:
    base_character_unit(
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
        std::string mount_name=""
    );
    virtual ~base_character_unit() = default;

    double points() const noexcept;
    std::vector<short> statistics() const noexcept;
    std::vector<std::string> special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;
    double magic_item_budget() const noexcept;
    double faction_item_budget() const noexcept;
    double total_item_budget() const noexcept;
    bool bsb() const noexcept;
    std::string mount_name() const noexcept;
};

#endif // !BASE_CHARACTER_UNIT_H
