#include "unit.h"

unit::unit() {}

unit::unit(const std::shared_ptr<base_unit>& _base)
    : base(_base) {
    unit_size = _base->min_size;
    current_points = _base->min_size * _base->pts_per_model;
    current_magic_item_points = 0.0;
    current_faction_item_points = 0.0;
    current_mage_level = _base->base_mage_level;
    current_weapon = {"", 0.0};
    current_armour[ArmourType::ARMOUR] = {"", 0.0};
    current_armour[ArmourType::SHIELD] = {"", 0.0};
    current_armour[ArmourType::HELMET] = {"", 0.0};
    current_mount = {"", 0.0};
    current_champion_weapon = {"", 0.0};
    current_champion_armour[ArmourType::ARMOUR] = {"", 0.0};
    current_champion_armour[ArmourType::SHIELD] = {"", 0.0};
    current_champion_armour[ArmourType::HELMET] = {"", 0.0};
}

std::string unit::name() const noexcept {
    return base->name;
}

armies::UnitType unit::type() const noexcept {
    return base->unit_type;
}

double unit::points() const noexcept {
    return current_points;
}

std::size_t unit::min_size() const noexcept {
    return base->min_size;
}

std::size_t unit::max_size() const noexcept {
    return base->max_size;
}

double unit::pick_weapon(ItemType item_type, std::string weapon) {
    double pts = 0.0;
    switch (item_type) {
    case ItemType::MUNDANE:
        if (!(base->opt_weapons.count(weapon)))
            throw std::invalid_argument("Weapon not found!");
        pts = base->opt_weapons[weapon];
        current_points -= unit_size * current_weapon.second;
        current_weapon = std::make_pair(weapon, pts);
        current_points += unit_size * pts;
        break;
    case ItemType::MAGIC:
        if (!(base->is_character || !(base->opt_command.count(CommandGroup::CHAMPION))))
            throw std::invalid_argument("Cannot give this unit a magic weapon.");
        if (base->is_character) {
            if (!((*(base->magic_items)).count(weapon)))
                throw std::invalid_argument("Weapon not found!");
            pts = (*(base->magic_items))[weapon].points;
            if (current_magic_item_points + pts > base->magic_item_budget)
                throw std::runtime_error("Magic item budget exceeded!");
            current_points -= unit_size * current_weapon.second;
            current_weapon = std::make_pair(weapon, pts);
            current_points += unit_size * pts;
        }
        else { // champion
            if (!((*(base->magic_items)).count(weapon)))
                throw std::invalid_argument("Weapon not found!");
            pts = (*(base->magic_items))[weapon].points;
            if (current_magic_item_points + pts > base->magic_item_budget)
                throw std::runtime_error("Magic item budget exceeded!");
            current_points -= current_champion_weapon.second;
            current_champion_weapon = std::make_pair(weapon, pts);
            current_points += pts;
            return pts;
        }
        break;
    case ItemType::FACTION:
        // TODO: implement once we have faction specific items parser + container
        break;
    case ItemType::COMMON:
        // TODO: implement once we have common item parser + container
        break;
    default:
        return 0.0;
    }
    return unit_size * pts;
}

double unit::pick_armour(ItemType item_type, ArmourType armour_type, std::string armour) {
    double pts = 0.0;
    switch (item_type) {
    case ItemType::MUNDANE:
        break;
    }
}

double unit::change_size(std::size_t size) {
    if (size < base->min_size || size > base->max_size)
        throw std::invalid_argument("Invalid unit size!");
    double before = current_points;
    current_points = size * base->pts_per_model;
    current_points += current_weapon.second * size;
    for (const auto& x : current_armour) current_points += x.second.second * size;
    current_points += current_mount.second * size;
    current_points += current_champion_weapon.second;
    for (const auto& x : current_champion_armour) current_points += x.second.second;
    // TODO: add the other options points values once implemented
    unit_size = size;
    return current_points - before;
}
