#ifndef OPTIONS_H
#define OPTIONS_H

#include "enums.h"
#include "mount.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct weapon_option {
    std::string name;
    WeaponType type;
    ItemCategory category;
    double points;
    std::vector<std::string> replacements;
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
};

struct armour_option {
    std::string name;
    ArmourType type;
    ItemCategory category;
    double points;
    std::vector<std::string> replacements;
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
};

struct extra_option {
    std::string name;
    double points;
    bool is_singular;
    std::vector<std::string> replacements;
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
};

struct budget {
    BudgetType type;
    double points;
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
};

struct tmp_parse_options {
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemCategory, double, std::vector<std::string>>
    > weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemCategory, double, std::vector<std::string>>
    > armour;
    std::unordered_map<
        std::string,
        double
    > mounts;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > oco_extras;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras;
};

class options {
private:
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemCategory, double, std::vector<std::string>>
    > weapons_;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemCategory, double, std::vector<std::string>>
    > armour_;
    std::unordered_map<
        std::string,
        double
    > mounts_;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > oco_extras_;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras_;
public:
    options();
    explicit options(
        std::unordered_map<
            std::string,
            std::tuple<WeaponType, ItemCategory, double, std::vector<std::string>>
        >&& _weapons,
        std::unordered_map<
            std::string,
            std::tuple<ArmourType, ItemCategory, double, std::vector<std::string>>
        >&& _armours,
        std::unordered_map<
            std::string,
            double
        >&& _mounts,
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >&& _oco_extras,
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >&& _mc_extras
    );
    explicit options(
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >&& _oco_extras,
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >&& _mc_extras
    );
    explicit options(tmp_parse_options&& tpo);
    ~options() = default;

    const std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemCategory, double, std::vector<std::string>>
    >& weapons() const noexcept;
    const std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemCategory, double, std::vector<std::string>>
    >& armour() const noexcept;
    const std::unordered_map<
        std::string,
        double
    >& mounts() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& oco_extras() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& mc_extras() const noexcept;
};

#endif // !OPTIONS_H
