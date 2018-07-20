#ifndef _BASE_UNIT_H
#define _BASE_UNIT_H

#include "army_maps.h"
#include "magic_item.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class _base_unit {
private:
    // enums
    armies::Faction faction_;
    armies::UnitType ut_;
    armies::UnitClass uc_;
    // basic
    std::string name_;
    std::size_t min_size_;
    std::size_t max_size_;

    // handle to magic items
    std::shared_ptr<
        std::unordered_map<std::string, magic_item>
    > magic_items;
public:
    _base_unit(
        armies::Faction faction,
        armies::UnitType ut,
        armies::UnitClass uc,
        std::string name,
        std::size_t min_size,
        std::size_t max_size = std::numeric_limits<std::size_t>::max()
    );
    virtual ~_base_unit() = default;

    armies::Faction faction() const noexcept;
    armies::UnitType unit_type() const noexcept;
    armies::UnitClass unit_class() const noexcept;

    std::string name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;

    void set_magic_item_handle(
        const std::shared_ptr<
            std::unordered_map<
                std::string,
                magic_item
            >
        >& items
    ) noexcept;
};

#endif // !_BASE_UNIT_H