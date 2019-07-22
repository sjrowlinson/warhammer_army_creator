#include "item.h"

item::item(
    ItemCategory ic,
    ItemType it,
    const std::string& nm,
    const std::string& descr,
    double pts,
    std::unordered_set<std::string> allowed,
    std::unordered_multimap<RestrictionField, std::any> restr,
    std::vector<std::string> rns
) : item_category_(ic), item_type_(it), name_(nm),
    description_(descr), points_(pts), allowed_units_(allowed),
    restrictions_(restr), runes_(rns) {}

ItemCategory item::category() const noexcept {
    return item_category_;
}
ItemType item::type() const noexcept {
    return item_type_;
}
const std::string& item::name() const noexcept {
    return name_;
}
const std::string& item::description() const noexcept {
    return description_;
}
double item::points() const noexcept {
    return points_;
}
const std::unordered_set<std::string>& item::allowed_units() const noexcept {
    return allowed_units_;
}
const std::unordered_multimap<RestrictionField, std::any>& item::restrictions() const noexcept {
    return restrictions_;
}