#ifndef UNIT_H
#define UNIT_H

class army_list;

#include "../base_units/base_unit.h"
#include "../base_units/base_mage_character_unit.h"
#include "../base_units/base_melee_character_unit.h"
#include "../base_units/base_normal_unit.h"
#include "../base_units/base_mixed_unit.h"
#include "../enums.h"

#include <any>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>

class unit {
private:
    int id_;
protected:
    ModelSelect model_select_;
    MixedSelect mixed_select_;
    double points_;
    // item points - refers to champion for non-character units
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;
    unsigned int n_magic_items;
    std::shared_ptr<base_unit> base_;
    army_list* army_;

    void do_replacements(const std::vector<std::string>& replacements, bool champion=false);
    virtual std::string restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        const std::string& item_name
    ) const;
    virtual std::string budget_restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        ItemType item_type
    ) const;

    std::string pick_magic_item(ItemType item_type, ItemCategory item_category, const std::string& name);

    virtual std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons_access() noexcept = 0;

    virtual
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour_access() noexcept = 0;

    virtual
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& talisman_access() noexcept = 0;

    virtual
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& enchanted_item_access() noexcept = 0;

    virtual
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item_access() noexcept = 0;

    virtual
    std::unordered_map<
        std::string,
        std::pair<ItemCategory, double>
    >& item_extras_access() noexcept = 0;

    virtual
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& magic_banner_acces() noexcept = 0;

public:
    explicit unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
    unit(const unit& other);
    virtual ~unit() = default;

    int id() const noexcept;
    void set_id(int id);

    virtual bool is_character() const noexcept = 0;
    virtual bool is_mage() const noexcept = 0;
    virtual bool is_mixed() const noexcept = 0;
    virtual bool has_non_duplicable_items() const noexcept;

    ModelSelect model_select() const noexcept;
    virtual bool switch_model_select(ModelSelect ms);

    MixedSelect mixed_select() const noexcept;
    virtual bool switch_mixed_select(MixedSelect ms);

    virtual double points() const noexcept;

    // const accessors for items
    virtual
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons() const noexcept = 0;
    virtual
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour() const noexcept = 0;
    virtual
    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& talisman() const noexcept = 0;
    virtual
    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& enchanted_item() const noexcept = 0;
    virtual
    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item() const noexcept = 0;
    virtual
    const std::unordered_map<
        std::string,
        std::pair<ItemCategory, double>
    >& item_extras() const noexcept = 0;
    virtual
    const std::pair<
        std::string,
        std::pair<bool, double>
    >& oco_extra() const noexcept = 0;
    virtual
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& mc_extras() const noexcept = 0;
    virtual const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept = 0;

    virtual double magic_item_points() const noexcept;
    virtual double faction_item_points() const noexcept;
    virtual double total_item_points() const noexcept;

    virtual const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept = 0;

    // basic properties
    const std::shared_ptr<base_unit>& base() const noexcept;
    BaseUnitType base_unit_type() const noexcept;
    const std::string& name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    Faction faction() const noexcept;
    UnitType unit_type() const noexcept;
    UnitCategory unit_class() const noexcept;

    // item selectors:

    // => weapon selection and removal
    virtual std::string pick_weapon(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_weapon(WeaponType wt, bool replacing=false) = 0;

    // => armour selection and removal
    virtual std::string pick_armour(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_armour(ArmourType at, bool replacing=false) = 0;

    // => talisman selection and removal [NOTE: always refers to champion for non-characters]
    virtual std::string pick_talisman(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_talisman();

    // => enchanted item selection and removal [NOTE: always refers to champion for non-characters]
    virtual std::string pick_enchanted_item(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_enchanted_item();

    // => arcane item selection and removal [NOTE: always refers to champion for non-characters]
    virtual std::string pick_arcane_item(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_arcane_item();

    // => other magic item selection and removal [NOTE: always refers to champion for non-characters]
    virtual std::string pick_magic_extra(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_magic_extra(const std::string& name);

    // => one-choice-only extra selection and removal
    virtual std::string pick_oco_extra(const std::string& name) = 0;
    virtual std::string remove_oco_extra() = 0;

    // => multiple-choice extra selection and removal
    virtual std::string pick_mc_extra(const std::string& name) = 0;
    virtual std::string remove_mc_extra(const std::string& name) = 0;

    // => mount selection and removal
    virtual void pick_mount(const std::string& name) = 0;
    virtual void remove_mount() = 0;
    // => mount options selection and removal
    virtual void pick_mount_option(const std::string& name, bool oco);
    virtual void remove_mount_option(const std::string& name, bool oco);

    // => banner selection and removal
    virtual std::string pick_banner(ItemCategory item_category, const std::string& name) = 0;
    virtual std::string remove_banner() = 0;

    // other modifiers
    virtual std::vector<std::string> clear();

    // serialisation and exporting
    virtual std::string save() const = 0;
};

#endif // !UNIT_H
