#include "army_list.h"

army_list::army_list(double points) :
    points(points), curr_pts(0.0), lord_lim(0.0),
    hero_lim(0.0), core_min(0.0), spec_lim(0.0),
    rare_lim(0.0), army(), lord_pts(0.0), hero_pts(0.0),
    core_pts(0.0), spec_pts(0.0), rare_pts(0.0),
    invalidities{InvalidListReason::CORE_MINIMUM}, snap_unit_pts(0.0) {
    determine_limits();
}

army_list::~army_list() {}

void army_list::add_unit(std::shared_ptr<unit> u) {
    auto pts = u->points();
    // too many points
    if (curr_pts + pts > points)
        invalidities.insert(InvalidListReason::POINTS);
    switch (u->unit_type()) {
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
    }
    army[u->id()] = u;
    curr_pts += pts;
    check_validity();
}

void army_list::remove_unit(int id) {
    auto pts = army[id]->points();
    armies::UnitType unit_type = army[id]->unit_type();
    army.erase(id);
    curr_pts -= pts;
    switch (unit_type) {
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
        spec_pts -= pts;
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
        if (x.second->unit_type() == armies::UnitType::LORD)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == armies::UnitType::LORD)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    lord_pts = 0.0;
    check_validity();
}

void army_list::remove_heroes() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::HERO)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == armies::UnitType::HERO)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    hero_pts = 0.0;
    check_validity();
}

void army_list::remove_core() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::CORE)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == armies::UnitType::CORE)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    core_pts = 0.0;
    check_validity();
}

void army_list::remove_special() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::SPECIAL)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == armies::UnitType::SPECIAL)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    spec_pts = 0.0;
    check_validity();
}

void army_list::remove_rare() {
    double pts = 0.0;
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::RARE)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == armies::UnitType::RARE)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    rare_pts = 0.0;
    check_validity();
}

double army_list::current_points() const noexcept { return curr_pts; }

std::size_t army_list::nlords() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == armies::UnitType::LORD; }
    ));
}

std::size_t army_list::nheroes() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == armies::UnitType::HERO; }
    ));
}

std::size_t army_list::ncore() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == armies::UnitType::CORE; }
    ));
}

std::size_t army_list::nspecial() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == armies::UnitType::SPECIAL; }
    ));
}

std::size_t army_list::nrare() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == armies::UnitType::RARE; }
    ));
}

double army_list::lord_points() const noexcept { return lord_pts; }
double army_list::hero_points() const noexcept { return hero_pts; }
double army_list::core_points() const noexcept { return core_pts; }
double army_list::special_points() const noexcept { return spec_pts; }
double army_list::rare_points() const noexcept { return rare_pts; }

const std::set<InvalidListReason>& army_list::invalid_reasons() const noexcept {
    return invalidities;
}

std::shared_ptr<unit> army_list::get_unit(int id) {
    switch (army[id]->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
        return std::dynamic_pointer_cast<melee_character_unit>(army[id]);
    case BaseUnitType::MAGE_CHARACTER:
        return std::dynamic_pointer_cast<mage_character_unit>(army[id]);
    case BaseUnitType::MIXED:
        return std::dynamic_pointer_cast<mixed_unit>(army[id]);
    case BaseUnitType::NORMAL:
        return std::dynamic_pointer_cast<normal_unit>(army[id]);
    default:
        return army[id];
    }
    //return army[id];
}

void army_list::take_snapshot_of(int id) {
    snap_unit_pts = army[id]->points();
}

void army_list::update_on(int id) {
    double diff = army[id]->points() - snap_unit_pts;
    curr_pts += diff;
    switch (army[id]->unit_type()) {
    case armies::UnitType::LORD:
        lord_pts += diff;
        break;
    case armies::UnitType::HERO:
        hero_pts += diff;
        break;
    case armies::UnitType::CORE:
        core_pts += diff;
        break;
    case armies::UnitType::SPECIAL:
        spec_pts += diff;
        break;
    case armies::UnitType::RARE:
        rare_pts += diff;
        break;
    default: break;
    }
    check_validity();
}

void army_list::change_points_limit(double pts) {
    points = pts;
    determine_limits();
    check_validity();
}

void army_list::clear() {
    army.clear();
    invalidities.clear();
    invalidities.insert(InvalidListReason::CORE_MINIMUM);
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

std::string army_list::html_lords_table() const {
    std::vector<std::string> headers = {
        "Unit", "Mount", "Wizard Level", "Weapons", "Armour", "Talisman", "Enchanted Item",
        "Arcane Item", "Other Magic/Faction Items", "Other Extras", "Banner", "Characteristics",
        "Points"
    };
    std::string table = "<table border=1 cellspacing=1 cellpadding=2>\n";
    table += "<thead><tr style=\"background-color: #000000\"><th colspan=\"13\" \
            style=\"color: #FFFFFF\">Lords</th></tr></thead>\n";
    table += "<thead><tr style=\"background-color: #C0C0C0\">";
    for (const auto& h : headers) table += "<th style=\"color: #FFFFFF\">" + h + "</th>\n";
    table += "</tr></thead>\n";
    //table += "<tbody>";
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::LORD)
            table += std::dynamic_pointer_cast<character_unit>(x.second)->html_table_row();
    }
    table += "</table>";
    return table;
}

std::string army_list::html_heroes_table() const {
    std::vector<std::string> headers = {
        "Unit", "Mount", "Wizard Level", "Weapons", "Armour", "Talisman", "Enchanted Item",
        "Arcane Item", "Other Magic/Faction Items", "Other Extras", "Banner", "Characteristics",
        "Points"
    };
    std::string table = "<table border=1 cellspacing=1 cellpadding=2>\n";
    table += "<thead><tr style=\"background-color: #000000\"><th colspan=\"13\" \
            style=\"color: #FFFFFF\">Heroes</th></tr></thead>\n";
    table += "<thead><tr style=\"background-color: #C0C0C0\">";
    for (const auto& h : headers) table += "<th style=\"color: #FFFFFF\">" + h + "</th>\n";
    table += "</tr></thead>\n";
    //table += "<tbody>";
    for (const auto& x : army) {
        if (x.second->unit_type() == armies::UnitType::HERO)
            table += std::dynamic_pointer_cast<character_unit>(x.second)->html_table_row();
    }
    table += "</table>";
    return table;
}
