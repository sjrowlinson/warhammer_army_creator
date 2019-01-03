#include "melee_character_unit.h"

melee_character_unit::melee_character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : character_unit(base, army_handle),
      handle(std::dynamic_pointer_cast<base_melee_character_unit>(base)) {}

melee_character_unit::melee_character_unit(const melee_character_unit& other)
    : character_unit(other), handle(other.handle) {}
