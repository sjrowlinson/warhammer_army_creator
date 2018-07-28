#include "melee_character_unit.h"

melee_character_unit::melee_character_unit(std::shared_ptr<base_unit> base)
    : character_unit(base),
      handle(std::dynamic_pointer_cast<base_melee_character_unit>(base)) {}
