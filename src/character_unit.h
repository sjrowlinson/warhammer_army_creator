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
        std::tuple<ItemClass, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours_;
    std::pair<std::string, std::pair<ItemClass, double>> talisman_;
    std::pair<std::string, std::pair<ItemClass, double>> enchanted_item_;
    std::pair<std::string, std::pair<bool, double>> oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras_;
    std::unordered_map<std::string, std::pair<ItemClass, double>> item_extras_;
    std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;
    std::pair<std::string, std::pair<ItemClass, double>> banner;

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;

    std::string pick_magic_item(ItemType item_type, ItemClass item_class, const std::string& name);
    std::string html_table_row_both(short mlevel, std::string arcane) const;

    std::pair<bool, std::string> restriction_check(
        RestrictionField picking,
        const std::unordered_map<RestrictionField, std::vector<std::string>>& restrictions
    ) const override;
public:
    const std::shared_ptr<base_character_unit> handle_;

    explicit character_unit(std::shared_ptr<base_unit> base);
    character_unit(const character_unit& other);
    virtual ~character_unit() override = default;

    bool is_character() const noexcept override;
    virtual bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;
    bool is_bsb() const noexcept;

    bool has_non_duplicable_items() const noexcept override;

    std::size_t size() const noexcept;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept override;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept override;

    std::pair<std::string, std::pair<ItemClass, double>> talisman() const noexcept;
    std::pair<std::string, std::pair<ItemClass, double>> enchanted_item() const noexcept;
    const std::unordered_map<std::string, std::pair<ItemClass, double>>& item_extras() const noexcept;

    std::pair<std::string, std::pair<bool, double>> oco_extra() const noexcept override;
    std::unordered_map<std::string, std::pair<bool, double>> mc_extras() const noexcept override;

    const std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    std::pair<std::string, std::pair<ItemClass, double>> magic_banner() const noexcept override;

    // current property modifiers
    std::string pick_weapon(ItemClass item_type, std::string name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;

    std::string pick_armour(ItemClass item_type, std::string name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;

    std::string pick_talisman(ItemClass item_class, std::string name);
    std::string remove_talisman();

    std::string pick_enchanted_item(ItemClass item_class, std::string name);
    std::string remove_enchanted_item();

    std::string pick_other(ItemClass item_class, std::string name);
    std::string remove_other(std::string name);

    std::string pick_oco_extra(std::string name) override;
    std::string remove_oco_extra() override;

    std::string pick_mc_extra(std::string name) override;
    std::string remove_mc_extra(std::string name) override;

    void pick_mount(std::string name) override;
    void remove_mount() override;

    std::string pick_banner(ItemClass item_class, std::string name) override;
    std::string remove_banner() override;

    void pick_mount_option(const std::string& name, bool oco) override;
    void remove_mount_option(const std::string& name, bool oco) override;

    virtual std::string html_table_row() const override;
};

#endif // !CHARACTER_UNIT_H
