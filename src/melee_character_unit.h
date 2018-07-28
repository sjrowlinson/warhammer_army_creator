#ifndef MELEE_CHARACTER_UNIT_H
#define MELEE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_melee_character_unit.h"
#include "character_unit.h"

#include <memory>

class melee_character_unit : public character_unit {
public:
    const std::shared_ptr<base_melee_character_unit> handle;

    explicit melee_character_unit(std::shared_ptr<base_unit> base);
    ~melee_character_unit() = default;
};

#endif // !MELEE_CHARACTER_UNIT_H
