#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "../enums.h"
#include "../base_units/base_unit.h"
#include "../base_units/base_normal_unit.h"
#include "unit.h"
#include "../tools.h"

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
    std::pair<std::string, std::pair<ItemCategory, double>> champ_talisman_;
    std::pair<std::string, std::pair<ItemCategory, double>> champ_enchanted_item_;
    std::pair<std::string, std::pair<ItemCategory, double>> champ_arcane_;
    std::unordered_map<std::string, std::pair<ItemCategory, double>> champ_item_extras_;
    std::pair<std::string, std::pair<bool, double>> champ_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> champ_mc_extras_;

    // mounts
    std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;

    // command
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > command_group;
    std::pair<std::string, std::pair<ItemCategory, double>> banner;

    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons_access() noexcept override;
    std::unordered_map<
            ArmourType,
            std::tuple<ItemCategory, std::string, double>
    >& armour_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& talisman_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& enchanted_item_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item_access() noexcept override;
    std::unordered_map<
        std::string,
        std::pair<ItemCategory, double>
    >& item_extras_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& magic_banner_access() noexcept override;
public:
    // handle
    const std::shared_ptr<base_normal_unit> handle;

    explicit normal_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
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
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& champion_weapons() const noexcept;

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

    const std::pair<std::string, std::pair<ItemCategory, double>>& talisman() const noexcept override;
    const std::pair<std::string, std::pair<ItemCategory, double>>& enchanted_item() const noexcept override;
    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item() const noexcept override;
    const std::unordered_map<std::string, std::pair<ItemCategory, double>>& item_extras() const noexcept override;

    const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& command() const noexcept;
    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    // item selectors:

    // => weapon selection and removal
    std::string pick_weapon(ItemCategory item_category, const std::string& name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;
    std::string pick_default_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_default_weapon(WeaponType wt, bool replacing=false);
    std::string pick_champion_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_champion_weapon(WeaponType wt, bool replacing=false);

    // => armour selection and removal
    std::string pick_armour(ItemCategory item_category, const std::string& name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;
    std::string pick_default_armour(ItemCategory item_category, const std::string& name);
    std::string remove_default_armour(ArmourType at, bool replacing=false);
    std::string pick_champion_armour(ItemCategory item_category, const std::string& name);
    std::string remove_champion_armour(ArmourType at, bool replacing=false);

    // => talisman selection and removal [NOTE: always refers to champion]
    std::string pick_talisman(ItemCategory item_category, const std::string& name) override;

    // => enchanted item selection and removal [NOTE: always refers to champion]
    std::string pick_enchanted_item(ItemCategory item_category, const std::string& name) override;

    // => arcane item selection and removal [NOTE: always refers to champion]
    virtual std::string pick_arcane_item(ItemCategory item_category, const std::string& name) override;

    // => other magic item selection and removal [NOTE: always refers to champion]
    std::string pick_magic_extra(ItemCategory item_category, const std::string& name) override;

    // => banner selection and removal
    std::string pick_banner(ItemCategory item_category, const std::string& name) override;
    std::string remove_banner() override;

    // => one-choice-only extra selection and removal
    std::string pick_oco_extra(const std::string& name) override;
    std::string remove_oco_extra() override;
    std::string pick_default_oco_extra(const std::string& name);
    std::string remove_default_oco_extra();
    std::string pick_champion_oco_extra(const std::string& name);
    std::string remove_champion_oco_extra();

    // => multiple-choice extra selection and removal
    std::string pick_mc_extra(const std::string& name) override;
    std::string remove_mc_extra(const std::string& name) override;
    std::string pick_default_mc_extra(const std::string& name);
    std::string remove_default_mc_extra(const std::string& name);
    std::string pick_champion_mc_extra(const std::string& name);
    std::string remove_champion_mc_extra(const std::string& name);

    // => mount selection and removal
    void pick_mount(const std::string& name) override;
    void remove_mount() override;

    // => command group selection and removal
    void add_command_member(CommandGroup member);
    void remove_command_member(CommandGroup member);

    // changing unit size
    void change_size(std::size_t n);

    // serialisation and exporting
    std::string save() override;
};

#endif // !_UNIT_H
