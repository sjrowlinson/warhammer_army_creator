#include "army_maps.h"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * TODO: the name argument to a unit should initiate look-up
 * to a map to load the properties of that unit - do not need
 * points_per_model nor min_size args as they can be obtained
 * from this process. 
 */

class unit {
protected:
    armies::Faction race;
    std::string name;
    std::size_t points_per_model;
    std::size_t size;
    std::size_t min_size;
    std::shared_ptr<
        std::unordered_map<
            std::string, armies::unit_properties
        >
    > army_map;
public:
    unit(armies::Faction faction,
         const std::string& name,
         std::size_t points_per_model,
         std::size_t size=0U,
         std::size_t min_size=0U);
    ~unit();
    const std::string& get_name() const noexcept;
    std::size_t points_value() const noexcept;
    std::size_t unit_size() const noexcept;
    std::size_t minimum_unit_size() const noexcept;
};
