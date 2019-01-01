#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "enums.h"
#include "base_unit.h"
#include "base_normal_unit.h"
#include "unit.h"
#include "tools.h"

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
        std::tuple<ItemCategory, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > armours_;
    std::pair<std::string, std::pair<bool, double>> oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras_;

    // champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > champ_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > champ_armours_;
    std::pair<std::string, std::pair<bool, double>> champ_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> champ_mc_extras_;
    double champ_magic_item_points;
    double champ_faction_item_points;
    double champ_total_item_points;

    // mounts
    std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;

    // command
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command_group;
    std::pair<std::string, std::pair<ItemCategory, double>> banner;
public:
    // handle
    const std::shared_ptr<base_normal_unit> handle;

    explicit normal_unit(const std::shared_ptr<base_unit>& base);
    normal_unit(const normal_unit& other);
    ~normal_unit() override = default;

    bool is_character() const noexcept override;
    bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;

    bool switch_model_select(ModelSelect ms) override;

    // current property accessors
    std::size_t size() const noexcept;
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons() const noexcept override;
    std::tuple<ItemCategory, std::string, double> melee_weapon() const;
    std::tuple<ItemCategory, std::string, double> ranged_weapon() const;
    std::tuple<ItemCategory, std::string, double> champion_melee_weapon() const;
    std::tuple<ItemCategory, std::string, double> champion_ranged_weapon() const;

    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour() const noexcept override;
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& champion_armour() const noexcept;

    const std::pair<std::string, std::pair<bool, double>>& oco_extra() const noexcept override;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& mc_extras() const noexcept override;
    const std::pair<std::string, std::pair<bool, double>>& champion_oco_extra() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& champion_mc_extras() const noexcept;

    const std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& command() const noexcept;
    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    // current property modifiers
    std::string pick_weapon(ItemCategory item_type, const std::string& name) override;
    std::string pick_armour(ItemCategory item_type, const std::string& name) override;

    std::string pick_default_weapon(ItemCategory item_type, const std::string& name);
    std::string pick_default_armour(ItemCategory item_type, const std::string& name);
    std::string pick_champion_weapon(ItemCategory item_type, const std::string& name);
    std::string pick_champion_armour(ItemCategory item_type, const std::string& name);

    std::string remove_weapon(WeaponType wt, bool replacing=false) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;

    std::string remove_default_weapon(WeaponType wt, bool replacing=false);
    std::string remove_default_armour(ArmourType at, bool replacing=false);
    std::string remove_champion_weapon(WeaponType wt, bool replacing=false);
    std::string remove_champion_armour(ArmourType at, bool replacing=false);

    void add_command_member(CommandGroup member);
    void remove_command_member(CommandGroup member);
    std::string pick_banner(ItemCategory item_type, const std::string& name) override;
    std::string remove_banner() override;

    std::string pick_oco_extra(const std::string& name) override;
    std::string pick_mc_extra(const std::string& name) override;

    std::string pick_default_oco_extra(const std::string& name);
    std::string pick_default_mc_extra(const std::string& name);
    std::string pick_champion_oco_extra(const std::string& name);
    std::string pick_champion_mc_extra(const std::string& name);

    std::string remove_oco_extra() override;
    std::string remove_mc_extra(const std::string& name) override;

    std::string remove_default_oco_extra();
    std::string remove_champion_oco_extra();
    std::string remove_default_mc_extra(const std::string& name);
    std::string remove_champion_mc_extra(const std::string& name);

    void pick_mount(const std::string& name) override;
    void remove_mount() override;

    void change_size(std::size_t n);

    std::string html_table_row() const override;

    virtual std::string save() const override;
};

#endif // !_UNIT_H
