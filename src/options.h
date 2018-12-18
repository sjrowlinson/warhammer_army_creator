#ifndef OPTIONS_H
#define OPTIONS_H

#include "enums.h"
#include "mount.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct tmp_parse_options {
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    > weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
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
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    > weapons_;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
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
            std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
        >&& _weapons,
        std::unordered_map<
            std::string,
            std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
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
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    >& weapons() const noexcept;
    const std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
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
