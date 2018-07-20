#ifndef _UNIT_H
#define _UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_normal_unit.h"
#include "base_mixed_unit.h"
#include "_base_unit.h"

#include <memory>

class _unit {
private:
    int id_;
protected:
    double points_;
    std::shared_ptr<_base_unit> base_;
public:
    explicit _unit(std::shared_ptr<_base_unit> base);
    _unit();
    virtual ~_unit() = default;

    int id() const noexcept;
    void set_id(int id);
    
    double points() const noexcept;

    BaseUnitType base_unit_type() const noexcept;
    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    armies::Faction faction() const noexcept;
    armies::UnitType unit_type() const noexcept;
    armies::UnitClass unit_class() const noexcept;
};

#endif // !_UNIT_H