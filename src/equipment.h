#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "enums.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class equipment {
private:
    std::unordered_map<
        WeaponType,
        std::pair<ItemClass, std::string>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::pair<ItemClass, std::string>
    > armour_;
    std::vector<std::string> talismans_;
    std::vector<std::string> arcane_;
    std::vector<std::string> enchanted_;
    std::vector<std::string> banners_;
public:
    equipment();
    explicit equipment(
        std::unordered_map<
            WeaponType,
            std::pair<ItemClass, std::string>
        >&& _weapons,
        std::unordered_map<
            ArmourType,
            std::pair<ItemClass, std::string>
        >&& _armours,
        std::vector<std::string>&& _talismans,
        std::vector<std::string>&& _arcanes,
        std::vector<std::string>&& _enchanteds,
        std::vector<std::string>&& _banners
    );
    ~equipment() = default;

    const std::unordered_map<
        WeaponType,
        std::pair<ItemClass, std::string>
    >& weapons() const noexcept;
    const std::unordered_map<
        ArmourType,
        std::pair<ItemClass, std::string>
    >& armours() const noexcept;
    const std::vector<std::string>& talismans() const noexcept;
    const std::vector<std::string>& arcane_items() const noexcept;
    const std::vector<std::string>& enchanted_items() const noexcept;
    const std::vector<std::string>& banners() const noexcept;
};

#endif // !EQUIPMENT_H
