#ifndef UNIT_H
#define UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "magic_item.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class unit {
private:
    std::shared_ptr<base_unit> base;
    int unit_id;
    std::size_t unit_size;

    double current_points;
    double current_magic_item_points;
    double current_faction_item_points;
    short current_mage_level;
    std::pair<ItemClass, std::pair<std::string, double>> current_melee_weapon;
    std::pair<ItemClass, std::pair<std::string, double>> current_ranged_weapon;
    std::unordered_map<
        ArmourType, std::tuple<ItemClass, std::string, double>
    > current_armour;
    std::pair<std::string, double> current_mount;

    // champion-specific
    std::pair<ItemClass, std::pair<std::string, double>> current_champion_weapon;
    std::unordered_map<
        ArmourType, std::pair<std::string, double>
    > current_champion_armour;

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > current_command;

    //void remove_current_weapon(bool is_champion);
public:
    unit();
    explicit unit(const std::shared_ptr<base_unit>& _base);
    ~unit() = default;

    int id() const noexcept;
    void set_id(int id);
    std::string name() const noexcept;
    armies::UnitType type() const noexcept;
    double points() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    std::size_t size() const noexcept;
    std::pair<std::string, double> melee_weapon() const noexcept;
    std::pair<std::string, double> ranged_weapon() const noexcept;
    std::unordered_map<
        ArmourType, std::tuple<ItemClass, std::string, double>
    > armour() const noexcept;

    // options
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > optional_weapons() const noexcept;

    //double pick_weapon(ItemClass item_type, std::string weapon);
    //double pick_armour(ItemClass item_type, ArmourType armour_type, std::string armour);
    double pick_mount(std::string mount);
    //double change_size(std::size_t size);
    double add_musician();
    double add_standard_bearer();
    double add_champion();
    double change_mage_level(short level);

    //void remove_weapon();
    //void remove_champion_weapon();
};

#endif // !UNIT_H
