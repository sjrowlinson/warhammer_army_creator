#include "army_list.h"

army_list::army_list(double points) :
    points(points), curr_pts(0.0), lord_lim(0.0),
    hero_lim(0.0), core_min(0.0), spec_lim(0.0),
    rare_lim(0.0), army(), item_tracker(), lord_pts(0.0), hero_pts(0.0),
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
    case UnitType::LORD:
        if (lord_pts + pts > lord_lim)
            invalidities.insert(InvalidListReason::LORD_LIMIT);
        lord_pts += pts;
        break;
    case UnitType::HERO:
        if (hero_pts + pts > hero_lim)
            invalidities.insert(InvalidListReason::HERO_LIMIT);
        hero_pts += pts;
        break;
    case UnitType::CORE:
        if (core_pts + pts < core_min)
            invalidities.insert(InvalidListReason::CORE_MINIMUM);
        core_pts += pts;
        break;
    case UnitType::SPECIAL:
        if (spec_pts + pts < spec_lim)
            invalidities.insert(InvalidListReason::SPECIAL_LIMIT);
        spec_pts += pts;
        break;
    case UnitType::RARE:
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
    UnitType unit_type = army[id]->unit_type();
    army.erase(id);
    curr_pts -= pts;
    switch (unit_type) {
    case UnitType::LORD:
        lord_pts -= pts;
        break;
    case UnitType::HERO:
        hero_pts -= pts;
        break;
    case UnitType::CORE:
        core_pts -= pts;
        break;
    case UnitType::SPECIAL:
        spec_pts -= pts;
        break;
    case UnitType::RARE:
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
        if (x.second->unit_type() == UnitType::LORD)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == UnitType::LORD)
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
        if (x.second->unit_type() == UnitType::HERO)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == UnitType::HERO)
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
        if (x.second->unit_type() == UnitType::CORE)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == UnitType::CORE)
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
        if (x.second->unit_type() == UnitType::SPECIAL)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == UnitType::SPECIAL)
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
        if (x.second->unit_type() == UnitType::RARE)
            pts += x.second->points();
    }
    for (auto it = std::begin(army); it != std::end(army);) {
        if (it->second->unit_type() == UnitType::RARE)
            it = army.erase(it);
        else ++it;
    }
    curr_pts -= pts;
    rare_pts = 0.0;
    check_validity();
}

double army_list::current_points() const noexcept { return curr_pts; }
double army_list::point_limit() const noexcept { return points; }

const std::unordered_map<std::string, unsigned int>& army_list::item_track_map() const noexcept {
    return item_tracker;
}

void army_list::incr_item_tracker(const std::string& s) {
    ++item_tracker[s];
}

void army_list::decr_item_tracker(const std::string& s) {
    --item_tracker[s];
    if (!item_tracker[s]) item_tracker.erase(s);
}

std::size_t army_list::nlords() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == UnitType::LORD; }
    ));
}

std::size_t army_list::nheroes() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == UnitType::HERO; }
    ));
}

std::size_t army_list::ncore() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == UnitType::CORE; }
    ));
}

std::size_t army_list::nspecial() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == UnitType::SPECIAL; }
    ));
}

std::size_t army_list::nrare() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->unit_type() == UnitType::RARE; }
    ));
}

double army_list::lord_points() const noexcept { return lord_pts; }
double army_list::hero_points() const noexcept { return hero_pts; }
double army_list::core_points() const noexcept { return core_pts; }
double army_list::special_points() const noexcept { return spec_pts; }
double army_list::rare_points() const noexcept { return rare_pts; }

bool army_list::has_bsb() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->mc_extras().count("Battle Standard Bearer"); }
    );
}

const std::set<InvalidListReason>& army_list::invalid_reasons() const noexcept {
    return invalidities;
}

std::shared_ptr<unit> army_list::get_unit(int id) {
    return army[id];
}

std::shared_ptr<const unit> army_list::get_unit(int id) const {
    return army.at(id);
}

void army_list::take_snapshot_of(int id) {
    snap_unit_pts = army[id]->points();
}

void army_list::update_on(int id) {
    double diff = army[id]->points() - snap_unit_pts;
    curr_pts += diff;
    switch (army[id]->unit_type()) {
    case UnitType::LORD:
        lord_pts += diff;
        break;
    case UnitType::HERO:
        hero_pts += diff;
        break;
    case UnitType::CORE:
        core_pts += diff;
        break;
    case UnitType::SPECIAL:
        spec_pts += diff;
        break;
    case UnitType::RARE:
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
    item_tracker.clear();
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

std::string army_list::html_table(UnitType ut) const {
    std::vector<std::string> headers;
    switch (ut) {
    case UnitType::LORD:
    case UnitType::HERO:
        headers = {
            "Unit", "Mount", "Wizard Level", "Weapons", "Armour", "Talisman", "Enchanted Item",
            "Arcane Item", "Other Magic/Faction Items", "Other Extras", "Banner", "Characteristics",
            "Points"
        };
        break;
    case UnitType::CORE:
    case UnitType::SPECIAL:
    case UnitType::RARE:
        headers = {
                "Unit", "Size", "Mount", "Weapons", "Armour", "Extras", "Command",
                "Banner", "Characteristics", "Points"
        };
        break;
    default: throw std::invalid_argument("Invalid unit type!");
    }
    std::string table = "<table border=1 cellspacing=1 cellpadding=2 width=100%>\n";
    // make unit type header
    table += "<thead><tr style=\"background-color: #000000\">\n";
    switch (ut) {
    case UnitType::LORD:
        table += "<th colspan=\"13\" style=\"color: #FFFFFF\">";
        table += "Lords (" + tools::points_str(lord_pts) + "/" + tools::points_str(lord_lim);
        break;
    case UnitType::HERO:
        table += "<th colspan=\"13\" style=\"color: #FFFFFF\">";
        table += "Heroes (" + tools::points_str(hero_pts) + "/" + tools::points_str(hero_lim);
        break;
    case UnitType::CORE:
        table += "<th colspan=\"10\" style=\"color: #FFFFFF\">";
        table += "Core Units (" + tools::points_str(core_pts);
        break;
    case UnitType::SPECIAL:
        table += "<th colspan=\"10\" style=\"color: #FFFFFF\">";
        table += "Special Units (" + tools::points_str(spec_pts) + "/" + tools::points_str(spec_lim);
        break;
    case UnitType::RARE:
        table += "<th colspan=\"10\" style=\"color: #FFFFFF\">";
        table += "Rare Units (" + tools::points_str(rare_pts) + "/" + tools::points_str(rare_lim);
        break;
    default: break;
    }
    table += " points)</th></tr></thead>\n";
    // make field headers
    table += "<thead><tr style=\"background-color: #C0C0C0\">";
    for (const auto& h : headers) table += "<th style = \"color: #FFFFFF\">" + h + "</th\n>";
    table += "</tr></thead>\n";
    // fill table
    for (const auto& x : army) {
        if (x.second->unit_type() == ut) table += x.second->html_table_row();
    }
    table += "</table>";
    return table;
}

std::string army_list::html_lords_table() const {
    return html_table(UnitType::LORD);
}

std::string army_list::html_heroes_table() const {
    return html_table(UnitType::HERO);
}

std::string army_list::html_core_table() const {
    return html_table(UnitType::CORE);
}

std::string army_list::html_special_table() const {
    return html_table(UnitType::SPECIAL);
}

std::string army_list::html_rare_table() const {
    return html_table(UnitType::RARE);
}

void army_list::save(const QString& filename) const {

}
