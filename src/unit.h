#ifndef UNIT_H
#define UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_normal_unit.h"
#include "base_mixed_unit.h"
#include "base_unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>

class unit {
private:
    int id_;
protected:
    double points_;
    std::shared_ptr<base_unit> base_;
public:
    explicit unit(std::shared_ptr<base_unit> base);
    unit(const unit& other);
    virtual ~unit() = default;

    int id() const noexcept;
    void set_id(int id);

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
    virtual
    std::pair<
        std::string,
        std::pair<armies::UnitClass, double>
    > mount() const noexcept = 0;

    // basic properties
    const std::shared_ptr<base_unit>& base() const noexcept;
    BaseUnitType base_unit_type() const noexcept;
    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    armies::Faction faction() const noexcept;
    armies::UnitType unit_type() const noexcept;
    armies::UnitClass unit_class() const noexcept;

    // pure virtual selectors
    virtual void pick_weapon(ItemClass item_type, std::string name) = 0;
    virtual void remove_weapon(WeaponType wt) = 0;
    virtual void pick_armour(ItemClass item_type, std::string name) = 0;
    virtual void remove_armour(ArmourType at) = 0;
    virtual void pick_oco_extra(std::string name) = 0;
    virtual void remove_oco_extra() = 0;
    virtual void pick_mc_extra(std::string name) = 0;
    virtual void remove_mc_extra(std::string name) = 0;
    virtual void pick_mount(std::string name) = 0;
    virtual void remove_mount() = 0;
};

#endif // !UNIT_H
