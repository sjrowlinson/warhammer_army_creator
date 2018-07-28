#ifndef CHARACTER_UNIT_H
#define CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_character_unit.h"
#include "unit.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

class character_unit : public unit {
protected:
    // equipment
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours_;
    std::pair<std::string, double> oco_extra_;
    std::unordered_map<std::string, double> mc_extras_;

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;

    const std::shared_ptr<base_character_unit> handle_;
public:
    explicit character_unit(std::shared_ptr<base_unit> base);
    virtual ~character_unit() = default;

    std::size_t size() const noexcept;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept;
    std::tuple<ItemClass, std::string, double> melee_weapon() const;
    std::tuple<ItemClass, std::string, double> ranged_weapon() const;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept;

    std::pair<std::string, double> oco_extra() const noexcept;
    std::unordered_map<std::string, double> mc_extras() const noexcept;

    // curremt property modifiers
    void pick_weapon(ItemClass item_type, std::string name);
    void pick_armour(ItemClass item_type, std::string name);
    void pick_oco_extra(std::string name);
    void pick_mc_extra(std::string name);
    void remove_weapon(WeaponType wt);
    void remove_armour(ArmourType at);
    void remove_oco_extra();
    void remove_mc_extra(std::string name);
};

#endif // !CHARACTER_UNIT_H