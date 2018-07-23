#ifndef ARMY_LIST_H
#define ARMY_LIST_H
#include "army_maps.h"
#include "unit.h"

#include "_unit.h"
#include "mage_character_unit.h"
#include "melee_character_unit.h"
#include "mixed_unit.h"
#include "normal_unit.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

enum class InvalidListReason {
    POINTS,
    LORD_LIMIT,
    HERO_LIMIT,
    CORE_MINIMUM,
    SPECIAL_LIMIT,
    RARE_LIMIT,
    VALID
};

class army_list {
private:
    double points;
    double curr_pts;
    double lord_lim;
    double hero_lim;
    double core_min;
    double spec_lim;
    double rare_lim;
    std::unordered_map<
        int,
        std::shared_ptr<_unit>
    > army;
    double lord_pts;
    double hero_pts;
    double core_pts;
    double spec_pts;
    double rare_pts;
    std::set<InvalidListReason> invalidities;

    void check_validity();
    void determine_limits();
public:
    army_list(double points);
    ~army_list();
    double current_points() const noexcept;
    // add/remove units
    void add_unit(std::shared_ptr<_unit> u);
    void remove_unit(int id);
    void remove_lords();
    void remove_heroes();
    void remove_core();
    void remove_special();
    void remove_rare();
    // get numbers of units
    std::size_t nlords() const noexcept;
    std::size_t nheroes() const noexcept;
    std::size_t ncore() const noexcept;
    std::size_t nspecial() const noexcept;
    std::size_t nrare() const noexcept;
    // unit type points
    double lord_points() const noexcept;
    double hero_points() const noexcept;
    double core_points() const noexcept;
    double special_points() const noexcept;
    double rare_points() const noexcept;

    std::shared_ptr<_unit> get_unit(int id);
    // list property modification
    void change_points_limit(double pts);
    // list modification
    void clear();
    // other
    bool is_valid() const noexcept;
};

#endif // !ARMY_LIST_H
