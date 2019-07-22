#ifndef CHARACTER_UNIT_H
#define CHARACTER_UNIT_H

#include "../enums.h"
#include "../base_units/base_unit.h"
#include "../base_units/base_character_unit.h"
#include "../mount.h"
#include "unit.h"
#include "../tools.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

class character_unit : public unit {
private:
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
protected:
    // equipment
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > armours_;
    std::pair<std::string, std::pair<ItemCategory, double>> talisman_;
    std::pair<std::string, std::pair<ItemCategory, double>> enchanted_item_;
    std::pair<std::string, std::pair<ItemCategory, double>> arcane_item_;
    std::pair<std::string, std::pair<bool, double>> oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras_;
    std::unordered_map<std::string, std::pair<ItemCategory, double>> item_extras_;
    std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;
    std::pair<std::string, std::pair<ItemCategory, double>> banner;

    virtual std::string restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        const std::string& item_name
    ) const override;
public:
    const std::shared_ptr<base_character_unit> handle_;

    explicit character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
    character_unit(const character_unit& other);
    virtual ~character_unit() override = default;

    bool is_character() const noexcept override;
    virtual bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;
    bool is_bsb() const noexcept;

    bool has_non_duplicable_items() const noexcept override;

    std::size_t size() const noexcept;
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons() const noexcept override;

    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour() const noexcept override;

    const std::pair<std::string, std::pair<ItemCategory, double>>& talisman() const noexcept override;
    const std::pair<std::string, std::pair<ItemCategory, double>>& enchanted_item() const noexcept override;
    virtual
    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item() const noexcept override;
    const std::unordered_map<std::string, std::pair<ItemCategory, double>>& item_extras() const noexcept override;

    const std::pair<std::string, std::pair<bool, double>>& oco_extra() const noexcept override;
    const std::unordered_map<std::string, std::pair<bool, double>>& mc_extras() const noexcept override;

    const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    // item selectors:

    // => weapon selection and removal
    std::string pick_weapon(ItemCategory item_category, const std::string& name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;

    // => armour selection and removal
    std::string pick_armour(ItemCategory item_category, const std::string& name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;

    std::string pick_talisman(ItemCategory item_category, const std::string& name) override;
    std::string pick_enchanted_item(ItemCategory item_category, const std::string& name) override;

    virtual std::string pick_arcane_item(ItemCategory item_category, const std::string& name) override;

    std::string pick_magic_extra(ItemCategory item_category, const std::string& name) override;

    // => one-choice-only extra selection and removal
    std::string pick_oco_extra(const std::string& name) override;
    std::string remove_oco_extra() override;

    // => multiple-choice extra selection and removal
    std::string pick_mc_extra(const std::string& name) override;
    std::string remove_mc_extra(const std::string& name) override;

    // => mount selection and removal
    void pick_mount(const std::string& name) override;
    void remove_mount() override;
    // => mount options selection and removal
    void pick_mount_option(const std::string& name, bool oco) override;
    void remove_mount_option(const std::string& name, bool oco) override;

    // => banner selection and removal
    std::string pick_banner(ItemCategory item_category, const std::string& name) override;
    std::string remove_banner() override;

    virtual std::vector<std::string> clear() override;

    virtual std::string save() const override;
};

#endif // !CHARACTER_UNIT_H
