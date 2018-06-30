#ifndef UNIT_H
#define UNIT_H

#include "army_maps.h"
#include "magic_item.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class unit {
protected:
    // constants
    armies::Faction race;
    armies::UnitType unit_type;
    armies::UnitClass unit_class;
    // base properties
    std::string name;
    double points_per_model;
    std::size_t size;
    std::size_t min_size;
    // mage related
    short mage_level;
    std::vector<std::pair<short, double>> mage_upgrades;
    std::vector<std::string> magic_lores;
    // statistic tables
    std::vector<short> stat_table;
    std::vector<short> champ_stat_table;
    // equipment and special rules
    std::vector<std::string> equipment;
    std::vector<std::string> special_rules;
    // optional items
    std::vector<std::pair<std::string, double>> optional_weapons;
    std::vector<std::pair<std::string, double>> optional_armour;
    std::vector<std::pair<std::string, double>> optional_mounts;
    std::vector<std::pair<std::string, double>> optional_extras;
    std::vector<std::pair<std::string, double>> optional_command;
    // magic item + faction-specific item budgets
    double magic_item_budget;
    double extra_item_budget;
    double total_item_budget;
    double magic_banner_budget;

    double current_points;
    // currently held options
    std::pair<std::string, double> current_weapon;
    // TODO: need to change for compatibility with armour + shield
    std::pair<std::string, double> current_armour;
    std::pair<std::string, double> current_mount;

    // handle to the magic items map
    std::shared_ptr<std::unordered_map<
        std::string,
        magic_item
    >> magic_items;
public:
    unit();
    unit(armies::Faction faction,
         armies::UnitType unit_type,
         armies::UnitClass unit_class,
         const std::string& name,
         double points_per_model,
         std::size_t size=0U,
         std::size_t min_size=1U);
    ~unit() = default;

    armies::UnitType get_type() const noexcept;
    // base properties
    const std::string& get_name() const noexcept;
    double base_points() const noexcept;
    double points() const noexcept;
    std::size_t unit_size() const noexcept;
    std::size_t minimum_unit_size() const noexcept;

    /* Initialising properties using parsed roster file */

    // mage related
    void init_mage_level(short level);
    void init_mage_level_upgrades(std::vector<std::pair<short, double>>&& levels);
    void init_magic_lores(std::vector<std::string>&& lores);
    // statistic tables
    void init_stat_table(std::vector<short>&& stats);
    void init_champion_stat_table(std::vector<short>&& stats);
    // equipment and special rules
    void init_equipment(std::vector<std::string>&& eq);
    void init_special_rules(std::vector<std::string>&& sr);
    // optional items
    void init_optional_weapons(std::vector<std::pair<std::string, double>>&& opt_weapons);
    void init_optional_armour(std::vector<std::pair<std::string, double>>&& opt_armour);
    void init_optional_mounts(std::vector<std::pair<std::string, double>>&& opt_mounts);
    void init_optional_extras(std::vector<std::pair<std::string, double>>&& opt_extras);
    void init_optional_command(std::vector<std::pair<std::string, double>>&& opt_command);
    // magic item + faction-specific item budgets
    void init_magic_item_budget(double budget);
    void init_extra_item_budget(double budget);
    void init_total_item_budget(double budget);
    void init_magic_banner_budget(double budget);

    void pass_magic_items_handle(std::shared_ptr<std::unordered_map<std::string, magic_item>> magic_items);

    /* Setting properties for an instance */
    double pick_optional_weapon(std::string weapon);
    double pick_optional_armour(std::string armour);
    double pick_optional_mount(std::string mount);
    double change_mage_level(short level);
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
