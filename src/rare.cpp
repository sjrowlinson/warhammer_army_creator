#include "rare.h"

rare::rare(const std::string& name,
           std::size_t points,
           std::size_t size,
           std::size_t min_size)
    : unit(name, points, size, min_size) {}

rare::~rare() {}
