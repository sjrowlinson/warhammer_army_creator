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
           std::size_t points_per_model,
           std::size_t size,
           std::size_t min_size)
    : race(faction),
      unit_type(unit_type),
      unit_class(unit_class),
      name(name),
      points_per_model(points_per_model),
      size(size),
      min_size(min_size),
      mage_level(0U) {}

std::size_t unit::points_value() const noexcept {
    return points_per_model * size;
}

armies::UnitType unit::get_type() const noexcept { return unit_type; }

const std::string& unit::get_name() const noexcept { return name; }

std::size_t unit::unit_size() const noexcept { return size; }

std::size_t unit::minimum_unit_size() const noexcept { return min_size; }

void unit::init_stat_table(std::vector<short>&& stats) {
    stat_table = std::move(stats);
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
void unit::init_magic_item_budget(std::size_t budget) {
    magic_item_budget = budget;
}
void unit::init_extra_item_budget(std::size_t budget) {
    extra_item_budget = budget;
}
void unit::init_total_item_budget(std::size_t budget) {
    total_item_budget = budget;
}
void unit::init_optional_extras(std::vector<std::pair<std::string, double>>&& opt_extras) {
    optional_extras = std::move(opt_extras);
}
