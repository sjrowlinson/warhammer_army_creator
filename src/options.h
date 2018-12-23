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
    std::unordered_map<std::string, weapon_option> weapons;
    std::unordered_map<std::string, armour_option> armour;
    std::unordered_map<std::string, double> mounts;
    std::unordered_map<std::string, extra_option> oco_extras;
    std::unordered_map<std::string, extra_option> mc_extras;
};

class options {
private:
    std::unordered_map<std::string, weapon_option> weapons_;
    std::unordered_map<std::string, armour_option> armour_;
    std::unordered_map<std::string, double> mounts_;
    std::unordered_map<std::string, extra_option> oco_extras_;
    std::unordered_map<std::string, extra_option> mc_extras_;
public:
    options();
    explicit options(tmp_parse_options&& tpo);
    ~options() = default;

    const std::unordered_map<std::string,  weapon_option>& weapons() const noexcept;
    const std::unordered_map<std::string, armour_option>& armour() const noexcept;
    const std::unordered_map<std::string, double>& mounts() const noexcept;
    const std::unordered_map<std::string, extra_option>& oco_extras() const noexcept;
    const std::unordered_map<std::string, extra_option>& mc_extras() const noexcept;
};

#endif // !OPTIONS_H
