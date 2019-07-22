#include "weapon.h"

weapon::weapon(
    WeaponType wt,
    ItemCategory ic,
    const std::string& nm,
    const std::string& descr,
    double pts,
    std::unordered_set<std::string> allowed,
    std::unordered_multimap<RestrictionField, std::any> restr,
    std::vector<std::string> rns
) : item(ic, ItemType::WEAPON, nm, descr, pts, allowed, restr, rns), weapon_type_(wt) {}

WeaponType weapon::weapon_type() const noexcept {
    return weapon_type_;
}