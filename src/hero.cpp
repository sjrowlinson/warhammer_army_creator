#include "hero.h"

hero::hero(armies::Faction faction,
           const std::string& name,
           std::size_t points,
           bool is_named=false)
    : unit(faction, name, points), is_named(is_named) {}

hero::~hero() {}

bool hero::special_character() const noexcept { return is_named; }
