#ifndef MELEE_CHARACTER_UNIT_H
#define MELEE_CHARACTER_UNIT_H

#include "../base_units/base_unit.h"
#include "../base_units/base_melee_character_unit.h"
#include "character_unit.h"

#include <memory>

class melee_character_unit : public character_unit {
public:
    const std::shared_ptr<base_melee_character_unit> handle;

    explicit melee_character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
    melee_character_unit(const melee_character_unit& other);
    ~melee_character_unit() = default;
};

#endif // !MELEE_CHARACTER_UNIT_H
