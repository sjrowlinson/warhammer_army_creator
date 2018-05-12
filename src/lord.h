#ifndef LORD_H
#define LORD_H
#include "unit.h"
#include <cstddef>
#include <string>

class lord : public unit {
private:
    bool is_named;
public:
    lord(armies::Faction faction,
         const std::string& name,
         std::size_t points,
         bool is_named);
    ~lord();
    bool special_character() const noexcept;
};

#endif //!LORD_H
