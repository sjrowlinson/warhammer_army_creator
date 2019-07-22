#include "armour.h"

armour::armour(
    ArmourType at,
    ItemCategory ic,
    const std::string& nm,
    const std::string& descr,
    double pts,
    std::unordered_set<std::string> allowed,
    std::unordered_multimap<RestrictionField, std::any> restr,
    std::vector<std::string> rns
) : item(ic, ItemType::ARMOUR, nm, descr, pts, allowed, restr, rns), armour_type_(at) {}

ArmourType armour::armour_type() const noexcept {
    return armour_type_;
}