#ifndef CORE_H
#define CORE_H
#include "unit.h"
#include <cstddef>
#include <string>

class core : public unit {
private:
    //
public:
    core(const std::string& name,
         std::size_t points,
         std::size_t size,
         std::size_t min_size);
    ~core();
};

#endif CORE_H
