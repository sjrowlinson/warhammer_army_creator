#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_normal_unit.h"
#include "_base_unit.h"
#include "_unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

class normal_unit : public _unit {
private:
    // TODO: consider making handle a const public field so that we
    // don't need base property accessor methods

    // handle
    std::shared_ptr<base_normal_unit> handle;

    // unit size
    std::size_t size_;

    // non-champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > extras_;

    // champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > champ_weapons;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > champ_armours;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > champ_extras;
    double champ_magic_item_points;
    double champ_faction_item_points;
    double champ_total_item_points;

    // command
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command_group;
    std::pair<std::string, double> banner;
public:
    explicit normal_unit(std::shared_ptr<_base_unit> base);
    ~normal_unit() = default;

    // current property accessors
    std::size_t size() const noexcept;
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

    // base property accessors
    const std::vector<short>& statistics() const noexcept;
    const std::vector<short>& champion_statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const std::vector<std::string>& champion_special_rules() const noexcept;
    const equipment& eq() const noexcept;
    const options& opt() const noexcept;
    const equipment& champion_eq() const noexcept;
    const options& champion_opt() const noexcept;
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& optional_command() const noexcept;
    double champion_magic_item_budget() const noexcept;
    double champion_faction_item_budget() const noexcept;
    double champion_total_item_budget() const noexcept;
    double magic_banner_budget() const noexcept;
};

#endif // !_UNIT_H