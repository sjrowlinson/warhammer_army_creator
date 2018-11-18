#ifndef MOUNT_H
#define MOUNT_H

#include "enums.h"

#include <string>
#include <unordered_map>
#include <vector>

class mount {
private:
    std::string name_;
    UnitClass unit_class_;
    std::vector<std::string> statistics_;
    std::vector<std::string> special_rules_;
    // one-choice-only extra
    std::unordered_map<std::string, double> oco_extras_;
    // multiple choice extras
    std::unordered_map<std::string, double> mc_extras_;
    // unit can only take the mount as an option if restrictions is either empty or
    // if the unit instance has all of the std::string instances corresponding to
    // a given RestrictionField - e.g. Disc of Tzeentch has:
    //     restrictions = {RestrictionField::OCO_EXTRA, {"Mark of Tzeentch"}};
    // such that a character can only choose a Disc of Tzeentch as a mount if they
    // have "Mark of Tzeentch" in their oco_extra field
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions_;
public:
    mount();
    explicit mount(
        std::string&& _name,
        UnitClass _uc,
        std::vector<std::string>&& _stats,
        std::vector<std::string>&& _sr,
        std::unordered_map<std::string, double>&& _oco_extras,
        std::unordered_map<std::string, double>&& _mc_extras,
        std::unordered_map<RestrictionField, std::vector<std::string>>&& _restrictions
    );
    ~mount() = default;

    const std::string& name() const noexcept;
    UnitClass unit_class() const noexcept;
    const std::vector<std::string>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
    const std::unordered_map<std::string, double>& oco_extras() const noexcept;
    const std::unordered_map<std::string, double>& mc_extras() const noexcept;
    const std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    >& restrictions() const noexcept;

    bool has_options() const noexcept;
};

#endif // !MOUNT_H
