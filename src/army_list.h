#ifndef ARMY_LIST_H
#define ARMY_LIST_H
#include "lord.h"
#include "hero.h"
#include "core.h"
#include "special.h"
#include "rare.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
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
    std::vector<std::shared_ptr<lord>> lords;
    std::vector<std::shared_ptr<hero>> heroes;
    std::vector<std::shared_ptr<core>> cores;
    std::vector<std::shared_ptr<special>> specials;
    std::vector<std::shared_ptr<rare>> rares;

    void determine_limits();
public:
    army_list(std::size_t points);
    ~army_list();
    std::size_t current_points() const noexcept;
    // add units
    void add_lord(const std::shared_ptr<lord>& _lord);
    void add_hero(const std::shared_ptr<hero>& _hero);
    void add_core(const std::shared_ptr<core>& _core);
    void add_special(const std::shared_ptr<special>& _special);
    void add_rare(const std::shared_ptr<rare>& _rare);
    // get numbers of units
    std::size_t nlords() const noexcept;
    std::size_t nheroes() const noexcept;
    std::size_t ncore() const noexcept;
    std::size_t nspecial() const noexcept;
    std::size_t nrare() const noexcept;
    // list property modification
    void change_points_limit(std::size_t pts);
};

#endif // !ARMY_LIST_H
