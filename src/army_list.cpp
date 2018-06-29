#include "army_list.h"

army_list::army_list(double points) : points(points) {
    determine_limits();
}

army_list::~army_list() {}

void army_list::add_unit(const std::shared_ptr<unit>& _unit) {
    auto pts = _unit->points();
    // too many points
    if (curr_pts + pts > points)
        invalidities.insert(InvalidListReason::POINTS);
    switch (_unit->get_type()) {
    case armies::UnitType::LORD:
        if (lord_pts + pts > lord_lim)
            invalidities.insert(InvalidListReason::LORD_LIMIT);
        lord_pts += pts;
        break;
    case armies::UnitType::HERO:
        if (hero_pts + pts > hero_lim)
            invalidities.insert(InvalidListReason::HERO_LIMIT);
        hero_pts += pts;
        break;
    case armies::UnitType::CORE:
        if (core_pts + pts < core_min)
            invalidities.insert(InvalidListReason::CORE_MINIMUM);
        core_pts += pts;
        break;
    case armies::UnitType::SPECIAL:
        if (spec_pts + pts < spec_lim)
            invalidities.insert(InvalidListReason::SPECIAL_LIMIT);
        spec_pts += pts;
        break;
    case armies::UnitType::RARE:
        if (rare_pts + pts < rare_lim)
            invalidities.insert(InvalidListReason::RARE_LIMIT);
        rare_pts += pts;
        break;
    default:
        throw std::invalid_argument("unit type not recognised, cannot add unit to army list.");
        break;
    }
    army.push_back(_unit);
    curr_pts += pts;
}

void army_list::remove_unit(const std::shared_ptr<unit>& _unit) {
    auto pts = _unit->points();
    army.erase(
        std::remove(
            std::begin(army),
            std::end(army),
            _unit
        )
    );
    curr_pts -= pts;
    switch (_unit->get_type()) {
    case armies::UnitType::LORD:
        lord_pts -= pts;
        break;
    case armies::UnitType::HERO:
        hero_pts -= pts;
        break;
    case armies::UnitType::CORE:
        core_pts -= pts;
        break;
    case armies::UnitType::SPECIAL:
        hero_pts -= pts;
        break;
    case armies::UnitType::RARE:
        rare_pts -= pts;
        break;
    default:
        throw std::invalid_argument("unit type not recognised, cannot remove unit.");
    }
    check_validity();
}

void army_list::remove_lords() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x->get_type() == armies::UnitType::LORD)
            pts += x->points();
    }
    army.erase(
        std::remove_if(
            std::begin(army),
            std::end(army),
            [](const auto& x) { return x->get_type() == armies::UnitType::LORD; }
        )
    );
    curr_pts -= pts;
    lord_pts = 0.0;
    check_validity();
}

void army_list::remove_heroes() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x->get_type() == armies::UnitType::HERO)
            pts += x->points();
    }
    army.erase(
        std::remove_if(
            std::begin(army),
            std::end(army),
            [](const auto& x) { return x->get_type() == armies::UnitType::HERO; }
        )
    );
    curr_pts -= pts;
    hero_pts = 0.0;
    check_validity();
}

void army_list::remove_core() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x->get_type() == armies::UnitType::CORE)
            pts += x->points();
    }
    army.erase(
        std::remove_if(
            std::begin(army),
            std::end(army),
            [](const auto& x) { return x->get_type() == armies::UnitType::CORE; }
        )
    );
    curr_pts -= pts;
    core_pts = 0.0;
    check_validity();
}

void army_list::remove_special() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x->get_type() == armies::UnitType::SPECIAL)
            pts += x->points();
    }
    army.erase(
        std::remove_if(
            std::begin(army),
            std::end(army),
            [](const auto& x) { return x->get_type() == armies::UnitType::SPECIAL; }
        )
    );
    curr_pts -= pts;
    spec_pts = 0.0;
    check_validity();
}

void army_list::remove_rare() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x->get_type() == armies::UnitType::RARE)
            pts += x->points();
    }
    army.erase(
        std::remove_if(
            std::begin(army),
            std::end(army),
            [](const auto& x) { return x->get_type() == armies::UnitType::RARE; }
        )
    );
    curr_pts -= pts;
    rare_pts = 0.0;
    check_validity();
}

double army_list::current_points() const noexcept { return curr_pts; }

std::size_t army_list::nlords() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const std::shared_ptr<unit>& x) { return x->get_type() == armies::UnitType::LORD; }
    );
}

std::size_t army_list::nheroes() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const std::shared_ptr<unit>& x) { return x->get_type() == armies::UnitType::HERO; }
    );
}

std::size_t army_list::ncore() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const std::shared_ptr<unit>& x) { return x->get_type() == armies::UnitType::CORE; }
    );
}

std::size_t army_list::nspecial() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const std::shared_ptr<unit>& x) { return x->get_type() == armies::UnitType::SPECIAL; }
    );
}

std::size_t army_list::nrare() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const std::shared_ptr<unit>& x) { return x->get_type() == armies::UnitType::RARE; }
    );
}

const std::vector<std::shared_ptr<unit>>& army_list::get() const noexcept {
    return army;
}


void army_list::change_points_limit(double pts) {
    points = pts;
    determine_limits();
    check_validity();
}

void army_list::clear() {
    army.clear();
    invalidities.clear();
    curr_pts = 0.0;
    lord_pts = 0.0;
    hero_pts = 0.0;
    core_pts = 0.0;
    spec_pts = 0.0;
    rare_pts = 0.0;
}

bool army_list::is_valid() const noexcept {
    return invalidities.empty();
}

void army_list::check_validity() {
    if (curr_pts > points)
        invalidities.insert(InvalidListReason::POINTS);
    else
        invalidities.erase(InvalidListReason::POINTS);
    if (lord_pts > lord_lim)
        invalidities.insert(InvalidListReason::LORD_LIMIT);
    else
        invalidities.erase(InvalidListReason::LORD_LIMIT);
    if (hero_pts > hero_lim)
        invalidities.insert(InvalidListReason::HERO_LIMIT);
    else
        invalidities.erase(InvalidListReason::HERO_LIMIT);
    if (core_pts < core_min)
        invalidities.insert(InvalidListReason::CORE_MINIMUM);
    else
        invalidities.erase(InvalidListReason::CORE_MINIMUM);
    if (spec_pts > spec_lim)
        invalidities.insert(InvalidListReason::SPECIAL_LIMIT);
    else
        invalidities.erase(InvalidListReason::SPECIAL_LIMIT);
    if (rare_pts > rare_lim)
        invalidities.insert(InvalidListReason::RARE_LIMIT);
    else
        invalidities.erase(InvalidListReason::RARE_LIMIT);
}

void army_list::determine_limits() {
    double percent_25 = 0.25*points;
    lord_lim = percent_25;
    hero_lim = percent_25;
    core_min = percent_25;
    spec_lim = 2.0*percent_25;
    rare_lim = percent_25;
}