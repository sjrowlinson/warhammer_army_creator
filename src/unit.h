#ifndef UNIT_H
#define UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_normal_unit.h"
#include "base_mixed_unit.h"
#include "base_unit.h"

#include <memory>

class unit {
private:
    int id_;
protected:
    double points_;
    std::shared_ptr<base_unit> base_;
public:
    explicit unit(std::shared_ptr<base_unit> base);
    unit();
    virtual ~unit() = default;

    int id() const noexcept;
    void set_id(int id);

    virtual double points() const noexcept;

    BaseUnitType base_unit_type() const noexcept;
    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;
    armies::Faction faction() const noexcept;
    armies::UnitType unit_type() const noexcept;
    armies::UnitClass unit_class() const noexcept;
};

#endif // !UNIT_H