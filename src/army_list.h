#ifndef ARMY_LIST_H
#define ARMY_LIST_H

#include "enums.h"
#include "tools.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QFile>
#include <QString>
#include <QTextStream>

class unit;

class army_list {
private:
    Faction faction_;
    // unit and item containers
    std::unordered_map<
        int,
        std::shared_ptr<unit>
    > army;
    std::unordered_set<std::string> unique_units;
    std::shared_ptr<unit> general_;
    std::unordered_map<std::string, unsigned int> item_tracker;
    // point values / limits
    double points;
    double curr_pts;
    double lord_lim;
    double hero_lim;
    double core_min;
    double spec_lim;
    double rare_lim;
    double lord_pts;
    double hero_pts;
    double core_pts;
    double spec_pts;
    double rare_pts;
    double lord_no_contrib_pts;
    double hero_no_contrib_pts;
    double core_no_contrib_pts;
    double spec_no_contrib_pts;
    double rare_no_contrib_pts;
    double snap_unit_pts;
    // other
    std::set<InvalidListReason> invalidities;

    void check_validity();
    void determine_limits();
public:
    explicit army_list(double points, Faction faction);
    ~army_list() = default;
    double current_points() const noexcept;
    double point_limit() const noexcept;
    const std::unordered_map<std::string, unsigned int>& item_track_map() const noexcept;
    void incr_item_tracker(const std::string& s);
    void decr_item_tracker(const std::string& s);
    // add/remove units
    void add_unit(std::shared_ptr<unit> u);
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
    double lord_no_contrib_points() const noexcept;
    double hero_no_contrib_points() const noexcept;
    double core_no_contrib_points() const noexcept;
    double special_no_contrib_points() const noexcept;
    double rare_no_contrib_points() const noexcept;
    // bsb flag methods
    bool has_bsb() const noexcept;

    bool contains(const std::string& name) const noexcept;

    const std::set<InvalidListReason>& invalid_reasons() const noexcept;

    std::shared_ptr<unit> get_unit(int id);
    std::shared_ptr<const unit> get_unit(int id) const;
    void set_as_general(int id);
    void take_snapshot_of(int id);
    void update_on(int id);
    // list property modification
    void change_points_limit(double pts);
    void switch_faction(Faction f);
    // list modification
    void clear();
    // other
    bool is_valid() const noexcept;

    void save(const QString& filename) const;

    std::string html_characters_table() const;
    std::string html_units_table() const;
};

#endif // !ARMY_LIST_H
