#ifndef UNIT_H
#define UNIT_H

#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_normal_unit.h"
#include "base_mixed_unit.h"
#include "base_unit.h"
#include "enums.h"

#include <memory>
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

    void do_replacements(const std::vector<std::string>& replacements, bool champion=false);
    virtual std::pair<bool, std::string> restriction_check(
        RestrictionField picking,
        const std::unordered_map<RestrictionField, std::vector<std::string>>& restrictions
    ) const = 0;
public:
    explicit unit(std::shared_ptr<base_unit> base);
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
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept = 0;
    virtual
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept = 0;
    virtual
    std::pair<
        std::string,
        std::pair<bool, double>
    > oco_extra() const noexcept = 0;
    virtual
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras() const noexcept = 0;
    virtual const std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept = 0;

    virtual std::pair<std::string, std::pair<ItemClass, double>> magic_banner() const noexcept = 0;

    // basic properties
    const std::shared_ptr<base_unit>& base() const noexcept;
    BaseUnitType base_unit_type() const noexcept;
    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    Faction faction() const noexcept;
    UnitType unit_type() const noexcept;
    UnitClass unit_class() const noexcept;

    // item selectors
    // TODO: change all of these to return value of remove_weapon - i.e.
    // return a string encoding the item removed (so the item being replaced
    // for pick methods and the item being directly removed for remove
    // methods) => may not need this signature for mounts though, only items
    virtual void pick_weapon(ItemClass item_type, std::string name) = 0;
    virtual std::string remove_weapon(WeaponType wt, bool replacing=false) = 0;
    virtual void pick_armour(ItemClass item_type, std::string name) = 0;
    virtual void remove_armour(ArmourType at, bool replacing=false) = 0;
    virtual void pick_oco_extra(std::string name) = 0;
    virtual void remove_oco_extra() = 0;
    virtual void pick_mc_extra(std::string name) = 0;
    virtual void remove_mc_extra(std::string name) = 0;
    virtual void pick_mount(std::string name) = 0;
    virtual void remove_mount() = 0;
    virtual void pick_banner(ItemClass item_class, std::string name) = 0;
    virtual void remove_banner() = 0;

    virtual void pick_mount_option(const std::string& name, bool oco);
    virtual void remove_mount_option(const std::string& name, bool oco);

    virtual std::string html_table_row() const = 0;
};

#endif // !UNIT_H
