#include "core.h"

core::core(armies::Faction faction,
           const std::string& name,
           std::size_t points,
           std::size_t size,
           std::size_t min_size)
    : unit(faction, name, points, size, min_size) {}

core::~core() {}
