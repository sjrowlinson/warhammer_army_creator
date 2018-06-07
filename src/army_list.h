#ifndef ARMY_LIST_H
#define ARMY_LIST_H
#include "army_maps.h"
#include "unit.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

class army_list {
private:
    std::size_t points;
    std::size_t curr_pts;
    std::size_t char_lim;
    std::size_t lord_lim;
    std::size_t hero_lim;
    std::size_t core_min;
    std::size_t spec_lim;
    std::size_t rare_lim;
    std::vector<std::shared_ptr<unit>> army;
    std::size_t num_lords;
    std::size_t num_heroes;
    std::size_t num_cores;
    std::size_t num_specials;
    std::size_t num_rares;

    void determine_limits();
public:
    army_list(std::size_t points);
    ~army_list();
    std::size_t current_points() const noexcept;
    // add units
    void add_unit(const std::shared_ptr<unit>& _unit);
    // get numbers of units
    std::size_t nlords() const noexcept;
    std::size_t nheroes() const noexcept;
    std::size_t ncore() const noexcept;
    std::size_t nspecial() const noexcept;
    std::size_t nrare() const noexcept;
    const std::vector<std::shared_ptr<unit>>& get() const noexcept;
    // list property modification
    void change_points_limit(std::size_t pts);
    // list modification
    void clear();
    // other
    bool is_valid() const noexcept;
};

#endif // !ARMY_LIST_H
