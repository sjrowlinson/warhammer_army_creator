#ifndef UNIT_H
#define UNIT_H

class army_list;

#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_normal_unit.h"
#include "base_mixed_unit.h"
#include "enums.h"

#include <any>
#include <memory>
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
    std::shared_ptr<base_unit> base_;
    army_list* army_;

    void do_replacements(const std::vector<std::string>& replacements, bool champion=false);
    virtual std::string restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        const std::string& item_name
    ) const;
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

    // non-pure virtual properties
    virtual double points() const noexcept;

    // pure virtual properties
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

    // item selectors
    virtual std::string pick_weapon(ItemCategory item_type, const std::string& name) = 0;
    virtual std::string remove_weapon(WeaponType wt, bool replacing=false) = 0;
    virtual std::string pick_armour(ItemCategory item_type, const std::string& name) = 0;
    virtual std::string remove_armour(ArmourType at, bool replacing=false) = 0;
    virtual std::string pick_oco_extra(const std::string& name) = 0;
    virtual std::string remove_oco_extra() = 0;
    virtual std::string pick_mc_extra(const std::string& name) = 0;
    virtual std::string remove_mc_extra(const std::string& name) = 0;
    virtual void pick_mount(const std::string& name) = 0;
    virtual void remove_mount() = 0;
    virtual std::string pick_banner(ItemCategory item_class, const std::string& name) = 0;
    virtual std::string remove_banner() = 0;

    virtual void pick_mount_option(const std::string& name, bool oco);
    virtual void remove_mount_option(const std::string& name, bool oco);

    // other modifiers
    virtual std::vector<std::string> clear();

    // serialisation and exporting
    virtual std::string html_table_row() const = 0;
    virtual std::string save() const = 0;
};

#endif // !UNIT_H
