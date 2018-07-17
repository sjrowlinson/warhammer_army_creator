#include "unit.h"

unit::unit() {}

unit::unit(const std::shared_ptr<base_unit>& _base)
    : base(_base) {
    unit_size = _base->min_size;
    current_points = _base->min_size * _base->pts_per_model;
    current_magic_item_points = 0.0;
    current_faction_item_points = 0.0;
    current_mage_level = _base->base_mage_level;

    current_melee_weapon = {
        _base->weapons[WeaponType::MELEE].first,
        {_base->weapons[WeaponType::MELEE].second, 0.0}
    };
    if (_base->weapons.count(WeaponType::BALLISTIC))
        current_ranged_weapon = {
            _base->weapons[WeaponType::BALLISTIC].first,
            {_base->weapons[WeaponType::BALLISTIC].second, 0.0}
        };
    else
        current_ranged_weapon = {ItemClass::MUNDANE, {"", 0.0}};
    if (_base->armour.count(ArmourType::ARMOUR))
        current_armour[ArmourType::ARMOUR] = {
            _base->armour[ArmourType::ARMOUR].first,
            _base->armour[ArmourType::ARMOUR].second,
            0.0
        };
    else
        current_armour[ArmourType::ARMOUR] = {ItemClass::MUNDANE ,"", 0.0};
    if (_base->armour.count(ArmourType::SHIELD))
        current_armour[ArmourType::SHIELD] = {
            _base->armour[ArmourType::SHIELD].first,
            _base->armour[ArmourType::SHIELD].second,
            0.0
        };
    else
        current_armour[ArmourType::SHIELD] = {ItemClass::MUNDANE, "", 0.0};
    if (_base->armour.count(ArmourType::HELMET))
        current_armour[ArmourType::HELMET] = {
            _base->armour[ArmourType::HELMET].first,
            _base->armour[ArmourType::HELMET].second,
            0.0
        };
    else
        current_armour[ArmourType::HELMET] = {ItemClass::MUNDANE, "", 0.0};

    current_mount = {"", 0.0};
    current_champion_weapon = {ItemClass::MUNDANE, {"Hand weapon", 0.0}};
    current_champion_armour[ArmourType::ARMOUR] = {"", 0.0};
    current_champion_armour[ArmourType::SHIELD] = {"", 0.0};
    current_champion_armour[ArmourType::HELMET] = {"", 0.0};
}

int unit::id() const noexcept { return unit_id; }

void unit::set_id(int id) { unit_id = id; }

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

std::size_t unit::size() const noexcept {
    return unit_size;
}

std::pair<std::string, double> unit::melee_weapon() const noexcept {
    if (current_melee_weapon.second.first.empty())
        return {"Hand weapon", 0.0};
    return current_melee_weapon.second;
}

std::pair<std::string, double> unit::ranged_weapon() const noexcept {
    return current_ranged_weapon.second;
}

std::unordered_map<
    ArmourType, std::tuple<ItemClass, std::string, double>
> unit::armour() const noexcept {
    return current_armour;
}

std::unordered_map<
    std::string,
    std::tuple<WeaponType, ItemClass, double>
> unit::optional_weapons() const noexcept {
    return base->opt_weapons;
}

/*void unit::remove_current_weapon(bool is_champion) {
    double pts = current_weapon.second.second;
    if (!is_champion) {
        switch (current_weapon.first) {
        case ItemClass::MAGIC:
        case ItemClass::COMMON:
            current_magic_item_points -= pts;
            break;
        case ItemClass::FACTION:
            current_faction_item_points -= pts;
            break;
        default: break;
        }
        current_points -= unit_size * pts;
        current_weapon = {ItemClass::MUNDANE, {"Hand weapon", 0.0}};
    }
    else {
        switch (current_champion_weapon.first) {
        case ItemClass::MAGIC:
        case ItemClass::COMMON:
            current_magic_item_points -= pts;
            break;
        case ItemClass::FACTION:
            current_faction_item_points -= pts;
            break;
        default: break;
        }
        current_points -= pts;
        current_champion_weapon = {ItemClass::MUNDANE, {"Hand weapon", 0.0}};
    }
}*/

// TODO: remove item_type param and store the ItemClass of a weapon
// when parsing so that all we need here is the name of the weapon
// as a string look-up
/*double unit::pick_weapon(ItemClass item_type, std::string weapon) {
    double pts = 0.0;
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        if (!(base->opt_weapons.count(weapon)))
            throw std::invalid_argument("Weapon not found!");
        auto type_pts_pair = base->opt_weapons[weapon];
        remove_current_weapon(false);
        current_weapon = std::make_pair(item_type, std::make_pair(weapon, type_pts_pair.second));
        current_points += unit_size * pts;
        break;
    }
    case ItemClass::MAGIC:
        if (!(base->is_character || !(base->opt_command.count(CommandGroup::CHAMPION))))
            throw std::invalid_argument("Cannot give this unit a magic weapon.");
        if (base->is_character) {
            if (!((*(base->magic_items)).count(weapon)))
                throw std::invalid_argument("Weapon not found!");
            pts = (*(base->magic_items))[weapon].points;
            if (current_magic_item_points + pts > base->magic_item_budget)
                throw std::runtime_error("Magic item budget exceeded!");
            remove_current_weapon(false);
            current_weapon = std::make_pair(item_type, std::make_pair(weapon, pts));
            current_points += unit_size * pts;
            current_magic_item_points += pts;
        }
        // TODO: move this to a separate method i.e. unit::pick_champion_weapon
        else { // champion
            if (!((*(base->magic_items)).count(weapon)))
                throw std::invalid_argument("Weapon not found!");
            pts = (*(base->magic_items))[weapon].points;
            if (current_magic_item_points + pts > base->magic_item_budget)
                throw std::runtime_error("Magic item budget exceeded!");
            remove_current_weapon(true);
            current_champion_weapon = std::make_pair(item_type, std::make_pair(weapon, pts));
            current_points += pts;
            return pts;
        }
        break;
    case ItemClass::FACTION:
        // TODO: implement once we have faction specific items parser + container
        break;
    case ItemClass::COMMON:
        // TODO: implement once we have common item parser + container
        break;
    default:
        return 0.0;
    }
    return unit_size * pts;
}*/

/*double unit::pick_armour(ItemClass item_type, ArmourType armour_type, std::string armour) {
    double pts = 0.0;
    switch (item_type) {
    case ItemClass::MUNDANE:
        break;
    }
    return pts;
}*/

double unit::pick_mount(std::string mount) {
    if (!(base->opt_mounts.count(mount)))
        throw std::invalid_argument("Mount not found!");
    double pts = (base->opt_mounts[mount]).second;
    current_points -= unit_size * current_mount.second;
    current_mount = std::make_pair(mount, pts);
    current_points += unit_size * pts;
    return unit_size * pts;
}

/*double unit::change_size(std::size_t size) {
    if (size < base->min_size || size > base->max_size)
        throw std::invalid_argument("Invalid unit size!");
    double before = current_points;
    current_points = size * base->pts_per_model;
    current_points += current_weapon.second.second * size;
    for (const auto& x : current_armour) current_points += x.second.second * size;
    current_points += current_mount.second * size;
    current_points += current_champion_weapon.second.second;
    for (const auto& x : current_champion_armour) current_points += x.second.second;
    // TODO: add the other options points values once implemented
    unit_size = size;
    return current_points - before;
}*/

double unit::add_musician() {
    if (!(base->opt_command.count(CommandGroup::MUSICIAN)))
        throw std::runtime_error("No musician available to this unit!");
    auto musician = base->opt_command[CommandGroup::MUSICIAN];
    current_command[CommandGroup::MUSICIAN] = musician;
    current_points += musician.second;
    return musician.second;
}

double unit::add_standard_bearer() {
    if (!(base->opt_command.count(CommandGroup::STANDARD_BEARER)))
        throw std::runtime_error("No standard bearer available to this unit!");
    auto sb = base->opt_command[CommandGroup::STANDARD_BEARER];
    current_command[CommandGroup::STANDARD_BEARER] = sb;
    current_points += sb.second;
    return sb.second;
}

double unit::add_champion() {
    if (!(base->opt_command.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion available to this unit!");
    auto champion = base->opt_command[CommandGroup::CHAMPION];
    current_command[CommandGroup::CHAMPION] = champion;
    current_points += champion.second;
    return champion.second;
}

double unit::change_mage_level(short level) {
    if (!(base->is_mage))
        throw std::runtime_error("Unit is not a mage!");
    if (!(base->mage_level_upgrades.count(level)))
        throw std::invalid_argument("Specified mage level change not available!");
    double pts = base->mage_level_upgrades[level];
    current_points += unit_size * pts;
    return unit_size * pts;
}

/*void unit::remove_weapon() {
    remove_current_weapon(false);
}

void unit::remove_champion_weapon() {
    remove_current_weapon(true);
}*/
