#ifndef NORMAL_UNIT_H
#define NORMAL_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_normal_unit.h"
#include "_base_unit.h"
#include "_unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

class normal_unit : public _unit {
private:
    // handle
    std::shared_ptr<base_normal_unit> handle;
    std::size_t size_;
    // TODO: store fields such as current melee weapon, current ranged weapon
    // and current armour for both champion and non-champion models
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours;
public:
    explicit normal_unit(std::shared_ptr<_base_unit> base);
    ~normal_unit() = default;

    const std::vector<short>& statistics() const noexcept;
    const std::vector<short>& champion_statistics() const noexcept;

    const std::vector<std::string>& special_rules() const noexcept;
};

#endif // !_UNIT_H