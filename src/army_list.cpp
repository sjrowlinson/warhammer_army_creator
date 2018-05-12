#include "army_list.h"

army_list::army_list(std::size_t points) 
    : points(points), curr_pts(0U) {
    determine_limits();
    lords.reserve(lord_lim);
    heroes.reserve(hero_lim);
    cores.reserve(core_min);
    specials.reserve(spec_lim);
    rares.reserve(rare_lim);
}

army_list::~army_list() {}

void army_list::add_lord(const std::shared_ptr<lord>& _lord) {
    if (lords.size() >= lord_lim ||
            lords.size() + heroes.size() >= char_lim)
        throw std::logic_error("Too many character units!");
    if (curr_pts + _lord->points_value() > points)
        throw std::logic_error("Not enough available points!");
    if (_lord->special_character() &&
            std::any_of(
                lords.begin(),
                lords.end(),
                [&_lord](const std::shared_ptr<lord>& l) { 
                    return l->get_name() == _lord->get_name(); 
                }
            )
        )
        throw std::logic_error("Cannot include two of the same named character!");
    lords.push_back(_lord);
    curr_pts += _lord->points_value();
}

void army_list::add_hero(const std::shared_ptr<hero>& _hero) {
    if (heroes.size() >= hero_lim ||
            lords.size() + heroes.size() >= char_lim)
        throw std::logic_error("Too many character units!");
    if (curr_pts + _hero->points_value() > points)
        throw std::logic_error("Not enough available points!");
    if (_hero->special_character() &&
            std::any_of(
                heroes.begin(),
                heroes.end(),
                [&_hero](const std::shared_ptr<hero>& h) {
                    return h->get_name() == _hero->get_name();
                }
            )
        )
        throw std::logic_error("Cannot include two of the same named character!");
    heroes.push_back(_hero);
    curr_pts += _hero->points_value();
}

void army_list::add_core(const std::shared_ptr<core>& _core) {
    if (curr_pts + _core->points_value() > points)
        throw std::logic_error("Not enough available points!");
    cores.push_back(_core);
    curr_pts += _core->points_value();
}

void army_list::add_special(const std::shared_ptr<special>& _special) {
    if (specials.size() >= spec_lim)
        throw std::logic_error("Too many special units!");
    if (curr_pts + _special->points_value() > points)
        throw std::logic_error("Not enough available points!");
    specials.push_back(_special);
    curr_pts += _special->points_value();
}

void army_list::add_rare(const std::shared_ptr<rare>& _rare) {
    if (rares.size() >= rare_lim)
        throw std::logic_error("Too many rare units!");
    if (curr_pts + _rare->points_value() > points)
        throw std::logic_error("Not enough available points!");
    rares.push_back(_rare);
    curr_pts += _rare->points_value();
}

std::size_t army_list::current_points() const noexcept { return curr_pts; }

std::size_t army_list::nlords() const noexcept { return lords.size(); }

std::size_t army_list::nheroes() const noexcept { return heroes.size(); }

std::size_t army_list::ncore() const noexcept { return cores.size(); }

std::size_t army_list::nspecial() const noexcept { return specials.size(); }

std::size_t army_list::nrare() const noexcept { return rares.size(); }

void army_list::change_points_limit(std::size_t pts) {
    determine_limits();
    points = pts;
}

void army_list::unit_options_event_fired(
    armies::UnitType unit_type,
    const std::shared_ptr<unit>& _unit
) {
    switch(unit_type) {
        case armies::UnitType::LORD:
            auto it = std::find(
                lords.begin(), lords.end(), _unit
            );
        case armies::UnitType::HERO:
        case armies::UnitType::CORE:
        case armies::UnitType::SPECIAL:
        case armies::UnitType::RARE:
    }
}

void army_list::clear() {
    lords.clear();
    heroes.clear();
    cores.clear();
    specials.clear();
    rares.clear();
    points = 0U;
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