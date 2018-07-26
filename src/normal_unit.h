#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_normal_unit.h"
#include "unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

class normal_unit : public unit {
private:
    // unit size
    std::size_t size_;

    // non-champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours_;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > extras_;

    // champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > champ_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > champ_armours_;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > champ_extras_;
    double champ_magic_item_points;
    double champ_faction_item_points;
    double champ_total_item_points;

    // command
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command_group;
    std::pair<std::string, double> banner;
public:
    // handle
    const std::shared_ptr<base_normal_unit> handle;

    explicit normal_unit(std::shared_ptr<base_unit> base);
    ~normal_unit() = default;

    // current property accessors
    std::size_t size() const noexcept;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept;
    std::tuple<ItemClass, std::string, double> melee_weapon() const;
    std::tuple<ItemClass, std::string, double> ranged_weapon() const;
    std::tuple<ItemClass, std::string, double> champion_melee_weapon() const;
    std::tuple<ItemClass, std::string, double> champion_ranged_weapon() const;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept;
        std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > champion_armour() const noexcept;

    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > extras() const noexcept;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > champion_extras() const noexcept;

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command() const noexcept;
    std::pair<std::string, double> magic_banner() const noexcept;

    // current property modifiers
    void pick_weapon(ItemClass item_type, std::string name);
    void pick_armour(ItemClass item_type, std::string name);
    void pick_champion_weapon(ItemClass item_type, std::string name);
    void pick_champion_armour(ItemClass item_type, std::string name);

    void remove_weapon(WeaponType wt);
    void remove_armour(ArmourType at);
    void remove_champion_weapon(WeaponType wt);
    void remove_champion_armour(ArmourType at);

    void add_command_member(CommandGroup member);
    void remove_command_member(CommandGroup member);
    void pick_banner(ItemClass item_type, std::string name);
    void remove_banner();

    void pick_extra(std::string name);
    void pick_champion_extra(std::string name);

    void remove_extra(std::string name);
    void remove_champion_extra(std::string name);

    void change_size(std::size_t n);

};

#endif // !_UNIT_H
