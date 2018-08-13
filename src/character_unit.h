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
    std::pair<std::string, double> talisman_;
    std::pair<std::string, double> enchanted_item_;
    std::pair<std::string, std::pair<bool, double>> oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras_;
    std::pair<std::string, std::pair<armies::UnitClass, double>> mount_;

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;
public:
    const std::shared_ptr<base_character_unit> handle_;

    explicit character_unit(std::shared_ptr<base_unit> base);
    character_unit(const character_unit& other);
    virtual ~character_unit() override = default;

    std::size_t size() const noexcept;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept override;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept override;

    std::pair<std::string, double> talisman() const noexcept;
    std::pair<std::string, double> enchanted_item() const noexcept;

    std::pair<std::string, std::pair<bool, double>> oco_extra() const noexcept override;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras() const noexcept override;

    std::pair<
        std::string,
        std::pair<armies::UnitClass, double>
    > mount() const noexcept override;

    // current property modifiers
    void pick_weapon(ItemClass item_type, std::string name) override;
    void remove_weapon(WeaponType wt) override;

    void pick_armour(ItemClass item_type, std::string name) override;
    void remove_armour(ArmourType at) override;

    void pick_talisman(ItemClass item_class, std::string name);
    void remove_talisman();

    void pick_enchanted_item(ItemClass item_class, std::string name);
    void remove_enchanted_item();

    void pick_oco_extra(std::string name) override;
    void remove_oco_extra() override;

    void pick_mc_extra(std::string name) override;
    void remove_mc_extra(std::string name) override;

    void pick_mount(std::string name) override;
    void remove_mount() override;
};

#endif // !CHARACTER_UNIT_H
