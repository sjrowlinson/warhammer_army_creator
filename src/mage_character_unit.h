#ifndef MAGE_CHARACTER_UNIT_H
#define MAGE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "character_unit.h"

#include <memory>
#include <utility>

class mage_character_unit : public character_unit {
private:
    std::pair<short, double> level_;
public:
    const std::shared_ptr<base_mage_character_unit> handle;

    explicit mage_character_unit(std::shared_ptr<base_unit> base);
    ~mage_character_unit() = default;

    short level() const noexcept;
    void change_level(short lvl);
};

#endif // !MAGE_CHARACTER_UNIT_H
