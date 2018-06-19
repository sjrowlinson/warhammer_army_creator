#ifndef UNIT_H
#define UNIT_H
#include "army_maps.h"
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class unit {
protected:
    armies::Faction race;
    armies::UnitType unit_type;
    armies::UnitClass unit_class;
    std::string name;
    std::size_t points_per_model;
    std::size_t size;
    std::size_t min_size;
    std::size_t mage_level;
    std::vector<short> stat_table;
    std::vector<std::string> equipment;
    std::vector<std::string> special_rules;
    std::vector<std::pair<std::string, double>> optional_weapons;
    std::vector<std::pair<std::string, double>> optional_armour;
    std::vector<std::pair<std::string, double>> optional_mounts;
    std::vector<std::pair<std::string, double>> optional_extras;
    std::size_t magic_item_budget;
    std::size_t extra_item_budget;
    std::size_t total_item_budget;
public:
    unit();
    unit(armies::Faction faction,
         armies::UnitType unit_type,
         armies::UnitClass unit_class,
         const std::string& name,
         std::size_t points_per_model,
         std::size_t size=0U,
         std::size_t min_size=1U);
    ~unit() = default;
    armies::UnitType get_type() const noexcept;
    const std::string& get_name() const noexcept;
    std::size_t points_value() const noexcept;
    std::size_t unit_size() const noexcept;
    std::size_t minimum_unit_size() const noexcept;
    // modifiers
    void init_stat_table(std::vector<short>&& stats);
    void init_equipment(std::vector<std::string>&& eq);
    void init_special_rules(std::vector<std::string>&& sr);
    void init_optional_weapons(std::vector<std::pair<std::string, double>>&& opt_weapons);
    void init_optional_armour(std::vector<std::pair<std::string, double>>&& opt_armour);
    void init_optional_mounts(std::vector<std::pair<std::string, double>>&& opt_mounts);
    void init_magic_item_budget(std::size_t budget);
    void init_extra_item_budget(std::size_t budget);
    void init_total_item_budget(std::size_t budget);
    void init_optional_extras(std::vector<std::pair<std::string, double>>&& opt_extras);
};

// custom hash for unit to allow use in unordered containers
struct unit_hash {
    std::size_t operator()(const unit& _unit) const noexcept {
        std::size_t h1 = std::hash<std::string>{}(_unit.get_name());
        std::size_t h2 = std::hash<std::size_t>{}(_unit.unit_size());
        return h1 ^ (h2 << 1);
    }
};

#endif // !UNIT_H
