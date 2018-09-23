#ifndef BASE_UNIT_H
#define BASE_UNIT_H

#include "enums.h"
#include "magic_item.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct equipment {
    std::unordered_map<
        WeaponType,
        std::pair<ItemClass, std::string>
    > weapons;
    std::unordered_map<
        ArmourType,
        std::pair<ItemClass, std::string>
    > armour;
    std::vector<std::string> talismans;
    std::vector<std::string> arcane;
    std::vector<std::string> enchanted;
    std::vector<std::string> banners;

    equipment()
        : weapons(), armour(), talismans(), arcane(), enchanted(), banners() {}
};

struct options {
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    > opt_weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
    > opt_armour;
    std::unordered_map<
        std::string,
        std::pair<UnitClass, double>
    > opt_mounts;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > oco_extras;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras;
};

struct mount {
    std::string name;
    UnitClass unit_class;
    std::vector<std::string> statistics;
    std::vector<std::string> special_rules;
    // one-choice-only extra
    std::unordered_map<std::string, double> oco_extras;
    // multiple choice extras
    std::unordered_map<std::string, double> mc_extras;
    // unit can only take the mount as an option if restrictions is either empty or
    // if the unit instance has all of the std::string instances corresponding to
    // a given RestrictionField - e.g. Disc of Tzeentch has:
    //     restrictions = {RestrictionField::OCO_EXTRA, {"Mark of Tzeentch"}};
    // such that a character can only choose a Disc of Tzeentch as a mount if they
    // have "Mark of Tzeentch" in their oco_extra field
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
    bool has_options() const noexcept { return oco_extras.size() || mc_extras.size(); }
};

class base_unit {
protected:
    BaseUnitType but;
private:
    // enums
    Faction faction_;
    UnitType ut_;
    UnitClass uc_;
    // basic
    std::string name_;
    std::size_t min_size_;
    std::size_t max_size_;

    // handle to mounts
    std::shared_ptr<
        std::unordered_map<
            std::string, mount
        >
    > mounts;
    // handle to magic items
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > magic_items;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > common_items;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > faction_items;
public:
    explicit base_unit(
        Faction faction,
        UnitType ut,
        UnitClass uc,
        std::string name,
        std::size_t min_size,
        std::size_t max_size = std::numeric_limits<std::size_t>::max()
    );
    virtual ~base_unit() = default;

    BaseUnitType base_unit_type() const noexcept;

    Faction faction() const noexcept;
    UnitType unit_type() const noexcept;
    UnitClass unit_class() const noexcept;

    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;

    const std::shared_ptr<
        std::unordered_map<
            std::string, mount
        >
    >& mounts_handle() const noexcept;
    void set_mounts_handle(
        const std::shared_ptr<
           std::unordered_map<
               std::string, mount
           >
        >& mts
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& magic_items_handle() const noexcept;
    void set_magic_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& common_items_handle() const noexcept;
    void set_common_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& faction_items_handle() const noexcept;
    void set_faction_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
};

#endif // !BASE_UNIT_H
