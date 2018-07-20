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

struct options {
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
};

/*class _base_unit {
private:
protected:
public:
    _base_unit();
    virtual ~_base_unit() = default;
};

class base_normal_unit : public _base_unit {
public:
    base_normal_unit();
};

class base_mixed_unit : public _base_unit {
public:
    base_mixed_unit();
};

class base_character_unit : public _base_unit {
public:
    base_character_unit();
};

class base_melee_character_unit : public base_character_unit{
public:
    base_melee_character_unit();
};

class base_mage_character_unit : public base_character_unit {
public:
    base_mage_character_unit();
};*/

/**
 * Base class for instanced units - should store common current properties
 * like the id, the size of the unit and the points value.
 *
 * Also stores a handle to the base_unit containing all the base properties
 * of the unit (i.e. read-only properties such as the name, options, minimum
 * and maximum unit sizes etc.) Will need to use std::dynamic_pointer_cast
 * on this base object to get relevant properties for each unit type.
 */
/*class unit_object {
private:
    int id_;
    std::shared_ptr<_base_unit> base;
protected:
    std::size_t size_;
    double points_;
public:
    unit_object();
    virtual ~unit_object() = default;
};*/

/**
 * Inherits from unit_object. Represents a typical non-character, non-mixed unit
 * which can have an optional command group.
 */
/*class normal_unit : public unit_object {
private:
protected:
public:
    normal_unit();
};*/

/**
 * Inherits from unit_object. Represents a non-character unit, mixed unit which
 * can have optional command groups for both model types in the unit. This will
 * typically be the type used for units like Giant Rats, Rat Ogres and any other
 * mixed unit which can have "master" and "slaves".
 */
/*class mixed_unit : public unit_object {
public:
    mixed_unit();
};*/

/**
 * Inherits from unit_object. Represents a character unit.
 */
/*class character_unit : public unit_object {
public:
    character_unit();
};*/

/*class melee_character_unit : public character_unit {
public:
    melee_character_unit();
};

class mage_character_unit : public character_unit {
public:
    mage_character_unit();
};*/

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
