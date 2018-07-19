#ifndef BASE_UNIT_H
#define BASE_UNIT_H

#include "army_maps.h"
#include "magic_item.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class CommandGroup {
    MUSICIAN,
    STANDARD_BEARER,
    CHAMPION
};

struct equipment {
    std::unordered_map<
        WeaponType,
        std::pair<ItemClass, std::string>
    > weapons;
    std::unordered_map<
        ArmourType,
        std::pair<ItemClass, std::string>
    > armour;
    std::vector<std::string> talismans;
    std::vector<std::string> arcane;
    std::vector<std::string> enchanted;
    std::vector<std::string> banners;

    equipment()
        : weapons(), armour(), talismans(), arcane(), enchanted(), banners() {}
};

struct base_unit {
    // enum constants
    armies::Faction faction;
    armies::UnitType unit_type;
    armies::UnitClass unit_class;
    // basic properties
    std::string name;
    bool is_character;
    bool is_unique;
    bool is_mage;
    double pts_per_model;
    std::size_t min_size;
    std::size_t max_size;
    // statistic tables
    std::vector<short> stats;
    std::vector<short> champion_stats;
    // special rules
    std::vector<std::string> special_rules;

    // default equipment
    equipment eq;
    equipment champ_eq;

    // options
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > opt_weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > opt_armour;
    std::unordered_map<
        std::string,
        std::pair<armies::UnitClass, double>
    > opt_mounts;
    std::pair<
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >,
        bool
    > opt_extras;

    // optional command group
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > opt_command;

    // champion specific options

    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > champ_opt_weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > champ_opt_armour;
    std::pair<
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >,
        bool
    > champ_opt_extras;

    // magic item + faction-specific item/feature budgets
    double magic_item_budget;
    double faction_item_budget;
    double total_item_budget;
    double magic_banner_budget;

    // mage related
    short base_mage_level;
    std::unordered_map<short, double> mage_level_upgrades;
    std::vector<std::string> lores;

    // handle to magic items map
    std::shared_ptr<
        std::unordered_map<std::string, magic_item>
    > magic_items;
};

#endif // !BASE_UNIT_H
