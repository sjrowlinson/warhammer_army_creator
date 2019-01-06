#ifndef CHARACTER_UNIT_H
#define CHARACTER_UNIT_H

#include "enums.h"
#include "base_unit.h"
#include "base_character_unit.h"
#include "mount.h"
#include "unit.h"
#include "tools.h"

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
        std::tuple<ItemCategory, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > armours_;
    std::pair<std::string, std::pair<ItemCategory, double>> talisman_;
    std::pair<std::string, std::pair<ItemCategory, double>> enchanted_item_;
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

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;

    std::string pick_magic_item(ItemType item_type, ItemCategory item_class, const std::string& name);
    std::string html_table_row_both(short mlevel, std::string arcane) const;

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

    const std::pair<std::string, std::pair<ItemCategory, double>>& talisman() const noexcept;
    const std::pair<std::string, std::pair<ItemCategory, double>>& enchanted_item() const noexcept;
    const std::unordered_map<std::string, std::pair<ItemCategory, double>>& item_extras() const noexcept;

    const std::pair<std::string, std::pair<bool, double>>& oco_extra() const noexcept override;
    const std::unordered_map<std::string, std::pair<bool, double>>& mc_extras() const noexcept override;

    const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    double magic_item_points() const noexcept override;
    double faction_item_points() const noexcept override;
    double total_item_points() const noexcept override;

    // current property modifiers
    std::string pick_weapon(ItemCategory item_type, const std::string& name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;

    std::string pick_armour(ItemCategory item_type, const std::string& name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;

    std::string pick_talisman(ItemCategory item_class, const std::string& name);
    std::string remove_talisman();

    std::string pick_enchanted_item(ItemCategory item_class, const std::string& name);
    std::string remove_enchanted_item();

    std::string pick_other(ItemCategory item_class, const std::string& name);
    std::string remove_other(const std::string& name);

    std::string pick_oco_extra(const std::string& name) override;
    std::string remove_oco_extra() override;

    std::string pick_mc_extra(const std::string& name) override;
    std::string remove_mc_extra(const std::string& name) override;

    void pick_mount(const std::string& name) override;
    void remove_mount() override;

    std::string pick_banner(ItemCategory item_class, const std::string& name) override;
    std::string remove_banner() override;

    void pick_mount_option(const std::string& name, bool oco) override;
    void remove_mount_option(const std::string& name, bool oco) override;

    virtual std::vector<std::string> clear() override;

    virtual std::string html_table_row() const override;
    virtual std::string save() const override;
};

#endif // !CHARACTER_UNIT_H
