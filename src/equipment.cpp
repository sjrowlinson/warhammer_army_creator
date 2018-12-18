#include "equipment.h"

equipment::equipment()
    : weapons_(), armour_(), talismans_(),
      arcane_(), enchanted_(), banners_() {}

equipment::equipment(
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
) : weapons_(std::move(_weapons)),
    armour_(std::move(_armours)),
    talismans_(std::move(_talismans)),
    arcane_(std::move(_arcanes)),
    enchanted_(std::move(_enchanteds)),
    banners_(std::move(_banners)) {}

equipment::equipment(tmp_parse_equipment&& tpe)
    : weapons_(std::move(tpe.weapons)),
      armour_(std::move(tpe.armour)),
      talismans_(std::move(tpe.talismans)),
      arcane_(std::move(tpe.arcane)),
      enchanted_(std::move(tpe.enchanted)),
      banners_(std::move(tpe.banners)) {}

const std::unordered_map<
    WeaponType,
    std::pair<ItemClass, std::string>
>& equipment::weapons() const noexcept {
    return weapons_;
}
const std::unordered_map<
    ArmourType,
    std::pair<ItemClass, std::string>
>& equipment::armours() const noexcept {
    return armour_;
}
const std::vector<std::string>& equipment::talismans() const noexcept {
    return talismans_;
}
const std::vector<std::string>& equipment::arcane_items() const noexcept {
    return arcane_;
}
const std::vector<std::string>& equipment::enchanted_items() const noexcept {
    return enchanted_;
}
const std::vector<std::string>& equipment::banners() const noexcept {
    return banners_;
}
