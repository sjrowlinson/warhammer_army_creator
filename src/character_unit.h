#ifndef CHARACTER_UNIT_H
#define CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_character_unit.h"
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
    std::pair<std::string, std::pair<armies::UnitClass, double>> mount_;
    std::pair<std::string, std::pair<ItemClass, double>> banner;

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;

    void pick_magic_item(ItemType item_type, ItemClass item_class, const std::string& name);
    std::string html_table_row_both(short mlevel, std::string arcane) const;
public:
    const std::shared_ptr<base_character_unit> handle_;

    explicit character_unit(std::shared_ptr<base_unit> base);
    character_unit(const character_unit& other);
    virtual ~character_unit() override = default;

    bool is_character() const noexcept override;
    virtual bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;
    bool is_bsb() const noexcept;

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

    std::pair<
        std::string,
        std::pair<armies::UnitClass, double>
    > mount() const noexcept override;

    std::pair<std::string, std::pair<ItemClass, double>> magic_banner() const noexcept override;

    // current property modifiers
    void pick_weapon(ItemClass item_type, std::string name) override;
    void remove_weapon(WeaponType wt, bool replacing=false) override;

    void pick_armour(ItemClass item_type, std::string name) override;
    void remove_armour(ArmourType at, bool replacing=false) override;

    void pick_talisman(ItemClass item_class, std::string name);
    void remove_talisman();

    void pick_enchanted_item(ItemClass item_class, std::string name);
    void remove_enchanted_item();

    void pick_other(ItemClass item_class, std::string name);
    void remove_other(std::string name);

    void pick_oco_extra(std::string name) override;
    void remove_oco_extra() override;

    void pick_mc_extra(std::string name) override;
    void remove_mc_extra(std::string name) override;

    void pick_mount(std::string name) override;
    void remove_mount() override;

    void pick_banner(ItemClass item_class, std::string name) override;
    void remove_banner() override;

    virtual std::string html_table_row() const override;
};

#endif // !CHARACTER_UNIT_H
