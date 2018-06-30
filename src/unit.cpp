#include "unit.h"

unit::unit()
    : race(armies::Faction::NONE),
      unit_type(armies::UnitType::NONE),
      name(),
      points_per_model(0U),
      size(0U),
      min_size(0U) {}

unit::unit(armies::Faction faction,
           armies::UnitType unit_type,
           armies::UnitClass unit_class,
           const std::string& name,
           double points_per_model,
           std::size_t size,
           std::size_t min_size)
    : race(faction),
      unit_type(unit_type),
      unit_class(unit_class),
      name(name),
      points_per_model(points_per_model),
      current_points(points_per_model * size),
      size(size),
      min_size(min_size),
      mage_level(0U) {}

double unit::base_points() const noexcept {
    return points_per_model * size;
}

double unit::points() const noexcept {
    return current_points;
}

armies::UnitType unit::get_type() const noexcept { return unit_type; }

const std::string& unit::get_name() const noexcept { return name; }

std::size_t unit::unit_size() const noexcept { return size; }

std::size_t unit::minimum_unit_size() const noexcept { return min_size; }

void unit::pass_magic_items_handle(
    std::shared_ptr<std::unordered_map<
        std::string, magic_item
    >> _magic_items
) {
    magic_items = _magic_items;
}

void unit::init_stat_table(std::vector<short>&& stats) {
    stat_table = std::move(stats);
}
void unit::init_champion_stat_table(std::vector<short>&& stats) {
    champ_stat_table = std::move(stats);
}
void unit::init_equipment(std::vector<std::string>&& eq) {
    equipment = std::move(eq);
}
void unit::init_special_rules(std::vector<std::string>&& sr) {
    special_rules = std::move(sr);
}
void unit::init_optional_weapons(std::vector<std::pair<std::string, double>>&& opt_weapons) {
    optional_weapons = std::move(opt_weapons);
}
void unit::init_optional_armour(std::vector<std::pair<std::string, double>>&& opt_armour) {
    optional_armour = std::move(opt_armour);
}
void unit::init_optional_mounts(std::vector<std::pair<std::string, double>>&& opt_mounts) {
    optional_mounts = std::move(opt_mounts);
}
void unit::init_optional_command(std::vector<std::pair<std::string, double>>&& opt_command) {
    optional_command = std::move(opt_command);
}
void unit::init_magic_item_budget(double budget) {
    magic_item_budget = budget;
}
void unit::init_extra_item_budget(double budget) {
    extra_item_budget = budget;
}
void unit::init_total_item_budget(double budget) {
    total_item_budget = budget;
}
void unit::init_magic_banner_budget(double budget) {
    magic_banner_budget = budget;
}
void unit::init_optional_extras(std::vector<std::pair<std::string, double>>&& opt_extras) {
    optional_extras = std::move(opt_extras);
}

void unit::init_mage_level(short level) {
    mage_level = level;
}
void unit::init_mage_level_upgrades(std::vector<std::pair<short, double>>&& levels) {
    mage_upgrades = std::move(levels);
}
void unit::init_magic_lores(std::vector<std::string>&& lores) {
    magic_lores = std::move(lores);
}

double unit::pick_optional_weapon(std::string weapon) {
    // remove current weapon if present
    if (!(current_weapon.first.empty()))
        current_points -= size*current_weapon.second;
    // look for weapon in optional weapons first
    auto it = std::find_if(
        std::begin(optional_weapons),
        std::end(optional_weapons),
        [&weapon](const auto& x) { return x.first == weapon; }
    );
    if (it == std::end(optional_weapons)) { // then look in magic items map
        double pts = (*magic_items)[weapon].points;
        current_weapon = std::make_pair(weapon, pts);
        current_points += size*pts;
        return size * pts;
    }
    current_weapon = *it;
    current_points += size*it->second;
    return size*it->second;
}
double unit::pick_optional_armour(std::string armour) {
    // TODO: need to figure out what to do about armour + shield
    // options as can have both
    auto it = std::find_if(
        std::begin(optional_armour),
        std::end(optional_armour),
        [&armour](const auto& x) { return x.first == armour; }
    );
    if (it == std::end(optional_armour)) {
        double pts = (*magic_items)[armour].points;
        current_armour = std::make_pair(armour, pts);
        current_points += size*pts;
        return size * pts;
    }
    current_armour = *it;
    current_points += size*it->second;
    return size*it->second;
}
double unit::pick_optional_mount(std::string mount) {
    if (!(current_mount.first.empty()))
        current_points -= size*current_mount.second;
    auto it = std::find_if(
        std::begin(optional_mounts),
        std::end(optional_mounts),
        [&mount](const auto& x) { return x.first == mount; }
    );
    current_mount = *it;
    current_points += size*it->second;
    return size*it->second;
}
double unit::change_mage_level(short level) {
    auto it = std::find_if(
        std::begin(mage_upgrades),
        std::end(mage_upgrades),
        [&level](const auto& x) { return x.first == level; }
    );
    mage_level = it->first;
    current_points += size*it->second;
    return size*it->second;
}
