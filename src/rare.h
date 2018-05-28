#ifndef RARE_H
#define RARE_H
#include "unit.h"
#include <cstddef>
#include <string>

class rare : public unit {
private:
    //
public:
    rare(armies::Faction faction,
         const std::string& name,
         std::size_t points,
         std::size_t size,
         std::size_t min_size);
    ~rare();
};

#endif // !RARE_H
