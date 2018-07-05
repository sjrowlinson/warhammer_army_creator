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
#include <unordered_map>
#include <utility>
#include <vector>

class unit {
private:
    std::shared_ptr<base_unit> base;
    std::size_t unit_size;

    double current_points;
    double current_magic_item_points;
    double current_faction_item_points;
    short current_mage_level;
    std::pair<std::string, double> current_weapon;
    std::unordered_map<
        ArmourType, std::pair<std::string, double>
    > current_armour;
    std::pair<std::string, double> current_mount;
    // champion-specific
    std::pair<std::string, double> current_champion_weapon;
    std::unordered_map<
        ArmourType, std::pair<std::string, double>
    > current_champion_armour;
public:
    unit();
    explicit unit(const std::shared_ptr<base_unit>& _base);
    ~unit() = default;

    std::string name() const noexcept;
    armies::UnitType type() const noexcept;
    double points() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;

    double pick_weapon(ItemType item_type, std::string weapon);
    double pick_armour(ItemType item_type, ArmourType armour_type, std::string armour);
    double pick_mount(std::string mount);
    double change_mage_level(short level);
    double change_size(std::size_t size);
};

#endif // !UNIT_H
