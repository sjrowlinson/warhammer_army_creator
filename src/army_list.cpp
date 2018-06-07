#include "army_list.h"

army_list::army_list(std::size_t points)
    : points(points),
      curr_pts(0U),
      num_lords(0U),
      num_heroes(0U),
      num_cores(0U),
      num_specials(0U),
      num_rares(0U) {
    determine_limits();
}

army_list::~army_list() {}

void army_list::add_unit(const std::shared_ptr<unit>& _unit) {
    if (curr_pts + _unit->points_value() > points)
        throw std::logic_error("Not enough available points!");
    switch (_unit->get_type()) {
    case armies::UnitType::LORD:
        if (num_lords >= lord_lim ||
                num_lords + num_heroes >= char_lim)
            throw std::logic_error("Too many character units!");
        ++num_lords;
        break;
    case armies::UnitType::HERO:
        if (num_heroes >= hero_lim ||
                num_heroes + num_lords >= char_lim)
            throw std::logic_error("Too many character units!");
        ++num_heroes;
        break;
    case armies::UnitType::CORE:
        ++num_cores;
        break;
    case armies::UnitType::SPECIAL:
        if (num_specials >= spec_lim)
            throw std::logic_error("Too many special units!");
        ++num_specials;
        break;
    case armies::UnitType::RARE:
        if (num_rares >= rare_lim)
            throw std::logic_error("Too many rare units!");
        ++num_rares;
        break;
    default:
        break;
    }
    army.push_back(_unit);
    curr_pts += _unit->points_value();
}

std::size_t army_list::current_points() const noexcept { return curr_pts; }

std::size_t army_list::nlords() const noexcept { return num_lords; }

std::size_t army_list::nheroes() const noexcept { return num_heroes; }

std::size_t army_list::ncore() const noexcept { return num_cores; }

std::size_t army_list::nspecial() const noexcept { return num_specials; }

std::size_t army_list::nrare() const noexcept { return num_rares; }

const std::vector<std::shared_ptr<unit>>& army_list::get() const noexcept {
    return army;
}


void army_list::change_points_limit(std::size_t pts) {
    determine_limits();
    points = pts;
}

void army_list::clear() {
    army.clear();
    num_lords = 0U;
    num_heroes = 0U;
    num_cores = 0U;
    num_specials = 0U;
    num_rares = 0U;
    curr_pts = 0U;
}

bool army_list::is_valid() const noexcept {
    if (curr_pts > points
        || nlords() > lord_lim
        || nheroes() > hero_lim
        || nlords() + nheroes() > char_lim
        || ncore() < core_min
        || nspecial() > spec_lim
        || nrare() > rare_lim)
        return false;
    return true;
}

void army_list::determine_limits() {
    std::size_t normalised_pts = static_cast<std::size_t>(floor(points/1000.0));
    if (normalised_pts < 2U) {
        char_lim = 3U;
        lord_lim = 0U;
        hero_lim = 3U;
        core_min = 2U;
        spec_lim = 3U;
        rare_lim = 1U;
    }
    else {
        char_lim = 2U*normalised_pts;
        lord_lim = normalised_pts - 1U;
        hero_lim = 2U*normalised_pts;
        core_min = normalised_pts - 1U;
        spec_lim = normalised_pts + 2;
        rare_lim = normalised_pts;
    }
}