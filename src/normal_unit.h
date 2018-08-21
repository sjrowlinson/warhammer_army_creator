#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_normal_unit.h"
#include "unit.h"

#include <memory>
#include <stdexcept>
#include <string>
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
    std::pair<std::string, std::pair<bool, double>> oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras_;

    // champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > champ_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > champ_armours_;
    std::pair<std::string, double> champ_oco_extra_;
    std::unordered_map<std::string, double> champ_mc_extras_;
    double champ_magic_item_points;
    double champ_faction_item_points;
    double champ_total_item_points;

    // mounts
    std::pair<
        std::string,
        std::pair<armies::UnitClass, double>
    > mount_;

    // command
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command_group;
    std::pair<std::string, std::pair<ItemClass, double>> banner;
public:
    // handle
    const std::shared_ptr<base_normal_unit> handle;

    explicit normal_unit(std::shared_ptr<base_unit> base);
    normal_unit(const normal_unit& other);
    ~normal_unit() override = default;

    bool is_character() const noexcept override;
    bool is_mage() const noexcept override;

    bool switch_model_select(ModelSelect ms) override;

    // current property accessors
    std::size_t size() const noexcept;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept override;
    std::tuple<ItemClass, std::string, double> melee_weapon() const;
    std::tuple<ItemClass, std::string, double> ranged_weapon() const;
    std::tuple<ItemClass, std::string, double> champion_melee_weapon() const;
    std::tuple<ItemClass, std::string, double> champion_ranged_weapon() const;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept override;
        std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > champion_armour() const noexcept;

    std::pair<std::string, std::pair<bool, double>> oco_extra() const noexcept override;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras() const noexcept override;
    std::pair<std::string, double> champion_oco_extra() const noexcept;
    std::unordered_map<
        std::string,
        double
    > champion_mc_extras() const noexcept;

    std::pair<
        std::string,
        std::pair<armies::UnitClass, double>
    > mount() const noexcept override;

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command() const noexcept;
    std::pair<std::string, std::pair<ItemClass, double>> magic_banner() const noexcept;

    // current property modifiers
    void pick_weapon(ItemClass item_type, std::string name) override;
    void pick_armour(ItemClass item_type, std::string name) override;

    void pick_default_weapon(ItemClass item_type, std::string name);
    void pick_default_armour(ItemClass item_type, std::string name);
    void pick_champion_weapon(ItemClass item_type, std::string name);
    void pick_champion_armour(ItemClass item_type, std::string name);

    void remove_weapon(WeaponType wt, bool replacing=false) override;
    void remove_armour(ArmourType at, bool replacing=false) override;

    void remove_default_weapon(WeaponType wt, bool replacing=false);
    void remove_default_armour(ArmourType at, bool replacing=false);
    void remove_champion_weapon(WeaponType wt, bool replacing=false);
    void remove_champion_armour(ArmourType at, bool replacing=false);

    void add_command_member(CommandGroup member);
    void remove_command_member(CommandGroup member);
    void pick_banner(ItemClass item_type, std::string name);
    void remove_banner();

    void pick_oco_extra(std::string name) override;
    void pick_mc_extra(std::string name) override;

    void pick_default_oco_extra(std::string name);
    void pick_default_mc_extra(std::string name);
    void pick_champion_oco_extra(std::string name);
    void pick_champion_mc_extra(std::string name);

    void remove_oco_extra() override;
    void remove_mc_extra(std::string name) override;

    void remove_default_oco_extra();
    void remove_champion_oco_extra();
    void remove_default_mc_extra(std::string name);
    void remove_champion_mc_extra(std::string name);

    void pick_mount(std::string name) override;
    void remove_mount() override;

    void change_size(std::size_t n);

};

#endif // !_UNIT_H
