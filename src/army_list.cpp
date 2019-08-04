#include "army_list.h"
#include "units/unit.h"
#include "html_table_unit_row.h"

army_list::army_list(double points, Faction faction) : faction_(faction),
    army(), unique_units(), general_(), item_tracker(),
    points(points), curr_pts(0.0), lord_lim(0.0),
    hero_lim(0.0), core_min(0.0), spec_lim(0.0),
    rare_lim(0.0),  lord_pts(0.0), hero_pts(0.0),
    core_pts(0.0), spec_pts(0.0), rare_pts(0.0), lord_no_contrib_pts(0.0), hero_no_contrib_pts(0.0),
    core_no_contrib_pts(0.0), spec_no_contrib_pts(0.0), rare_no_contrib_pts(0.0), snap_unit_pts(0.0),
    invalidities{InvalidListReason::CORE_MINIMUM} {
    determine_limits();
}

void army_list::add_unit(std::shared_ptr<unit> u) {
    if (u->is_character() && std::dynamic_pointer_cast<base_character_unit>(u->base())->unique()) {
        if (unique_units.count(u->name()))
            throw std::invalid_argument("Army already contains " + u->name());
        unique_units.insert(u->name());
    }
    auto pts = u->points();
    // too many points
    if (curr_pts + pts > points)
        invalidities.insert(InvalidListReason::POINTS);
    switch (u->unit_type()) {
    case UnitType::LORD:
        if (u->base()->counts_towards()) {
            if (lord_pts + pts > lord_lim)
                invalidities.insert(InvalidListReason::LORD_LIMIT);
            lord_pts += pts;
        }
        else {
            lord_no_contrib_pts += pts;
        }
        break;
    case UnitType::HERO:
        if (u->base()->counts_towards()) {
            if (hero_pts + pts > hero_lim)
                invalidities.insert(InvalidListReason::HERO_LIMIT);
            hero_pts += pts;
        }
        else {
            hero_no_contrib_pts += pts;
        }
        break;
    case UnitType::CORE:
        if (u->base()->counts_towards()) {
            if (core_pts + pts < core_min)
                invalidities.insert(InvalidListReason::CORE_MINIMUM);
            core_pts += pts;
        }
        else {
            core_no_contrib_pts += pts;
        }
        break;
    case UnitType::SPECIAL:
        if (u->base()->counts_towards()) {
            if (spec_pts + pts < spec_lim)
                invalidities.insert(InvalidListReason::SPECIAL_LIMIT);
            spec_pts += pts;
        }
        else {
            spec_no_contrib_pts += pts;
        }
        break;
    case UnitType::RARE:
        if (u->base()->counts_towards()) {
            if (rare_pts + pts < rare_lim)
                invalidities.insert(InvalidListReason::RARE_LIMIT);
            rare_pts += pts;
        }
        else {
            rare_no_contrib_pts += pts;
        }
        break;
    default:
        throw std::invalid_argument("unit type not recognised, cannot add unit to army list.");
    }
    army[u->id()] = u;
    curr_pts += pts;
    check_validity();
}

void army_list::remove_unit(int id) {
    auto u = army[id];
    auto pts = u->points();
    if (u->is_character() &&
            std::dynamic_pointer_cast<base_character_unit>(u->base())->unique())
        unique_units.erase(u->name());
    UnitType unit_type = u->unit_type();
    auto items_to_decrement = u->clear();
    for (const auto& item : items_to_decrement) decr_item_tracker(item);
    army.erase(id);
    curr_pts -= pts;
    switch (unit_type) {
    case UnitType::LORD:
        if (u->base()->counts_towards()) {
            lord_pts -= pts;
        }
        else {
            lord_no_contrib_pts -= pts;
        }
        break;
    case UnitType::HERO:
        if (u->base()->counts_towards()) {
            hero_pts -= pts;
        }
        else {
            hero_no_contrib_pts -= pts;
        }
        break;
    case UnitType::CORE:
        if (u->base()->counts_towards()) {
            core_pts -= pts;
        }
        else {
            core_no_contrib_pts -= pts;
        }
        break;
    case UnitType::SPECIAL:
        if (u->base()->counts_towards()) {
            spec_pts -= pts;
        }
        else {
            spec_no_contrib_pts -= pts;
        }
        break;
    case UnitType::RARE:
        if (u->base()->counts_towards()) {
            rare_pts -= pts;
        }
        else {
            rare_no_contrib_pts -= pts;
        }
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

double army_list::lord_no_contrib_points() const noexcept { return lord_no_contrib_pts; }
double army_list::hero_no_contrib_points() const noexcept { return hero_no_contrib_pts; }
double army_list::core_no_contrib_points() const noexcept { return core_no_contrib_pts; }
double army_list::special_no_contrib_points() const noexcept { return spec_no_contrib_pts; }
double army_list::rare_no_contrib_points() const noexcept { return rare_no_contrib_pts; }

bool army_list::has_bsb() const noexcept {
    return std::count_if(
        std::begin(army),
        std::end(army),
        [](const auto& x) { return x.second->mc_extras().count("Battle Standard Bearer"); }
    );
}

bool army_list::contains(const std::string& name) const noexcept {
    return std::count_if(std::begin(army), std::end(army), [&name](const auto& x) {
        return name == x.second->name();
    });
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

void army_list::set_as_general(int id) {
    general_ = army[id];
}

void army_list::take_snapshot_of(int id) {
    snap_unit_pts = army[id]->points();
}

void army_list::update_on(int id) {
    auto u = army[id];
    double diff = u->points() - snap_unit_pts;
    curr_pts += diff;
    switch (u->unit_type()) {
    case UnitType::LORD:
        if (u->base()->counts_towards()) {
            lord_pts += diff;
        }
        else {
            lord_no_contrib_pts += diff;
        }
        break;
    case UnitType::HERO:
        if (u->base()->counts_towards()) {
            hero_pts += diff;
        }
        else {
            hero_no_contrib_pts += diff;
        }
        break;
    case UnitType::CORE:
        if (u->base()->counts_towards()) {
            core_pts += diff;
        }
        else {
            core_no_contrib_pts += diff;
        }
        break;
    case UnitType::SPECIAL:
        if (u->base()->counts_towards()) {
            spec_pts += diff;
        }
        else {
            spec_no_contrib_pts += diff;
        }
        break;
    case UnitType::RARE:
        if (u->base()->counts_towards()) {
            rare_pts += diff;
        }
        else {
            rare_no_contrib_pts += diff;
        }
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

void army_list::switch_faction(Faction f) {
    faction_ = f;
}

void army_list::clear() {
    army.clear();
    unique_units.clear();
    item_tracker.clear();
    invalidities.clear();
    invalidities.insert(InvalidListReason::CORE_MINIMUM);
    curr_pts = 0.0;
    lord_pts = 0.0;
    hero_pts = 0.0;
    core_pts = 0.0;
    spec_pts = 0.0;
    rare_pts = 0.0;
    lord_no_contrib_pts = 0.0;
    hero_no_contrib_pts = 0.0;
    core_no_contrib_pts = 0.0;
    spec_no_contrib_pts = 0.0;
    rare_no_contrib_pts = 0.0;
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
    const double percent_25 = 0.25*points;
    lord_lim = percent_25;
    hero_lim = percent_25;
    core_min = percent_25;
    spec_lim = 2.0*percent_25;
    rare_lim = percent_25;
}

std::string army_list::html_characters_table() const {
    const std::vector<std::string> headers = {
        "Unit", "Mount", "Wizard Level", "Weapons", "Armour", "Talisman", "Enchanted Item",
        "Arcane Item", "Other Magic/Faction Items", "Other Extras", "Banner", "Characteristics",
        "Points"
    };
    std::string table = "<table border=1 cellspacing=1 cellpadding=2 width=100%>\n"
                        "<thead><tr style=\"background-color: #000000\">\n"
                        "<th colspan=\"13\" style=\"color: #FFFFFF\">";
    table += "Lords (" + tools::points_str(lord_pts) + '/' + tools::points_str(lord_lim) + " points) & ";
    table += "Heroes (" + tools::points_str(hero_pts) + '/' + tools::points_str(hero_lim) + " points)";
    table += "</th></tr></thead>\n";
    // make field headers
    table += "<thead><tr style=\"background-color: #C0C0C0\">";
    for (const auto& h : headers) table += "<th style = \"color: #FFFFFF\">" + h + "</th\n>";
    table += "</tr></thead>\n";
    // fill table
    auto lords = tools::find_all_if(std::begin(army), std::end(army), [](const auto& x) {
        return x.second->unit_type() == UnitType::LORD;
    });
    auto heroes = tools::find_all_if(std::begin(army), std::end(army), [](const auto& x) {
        return x.second->unit_type() == UnitType::HERO;
    });
    for (const auto& x : lords) {
        html_table_unit_row htr(x->second);
        auto tr = htr.to_string();
        if (x->second == general_) {
            auto search_td = tr.find("td") + 3U;
            tr.insert(search_td, "<em>");
            tr.insert(search_td + 4U + x->second->name().size(), "</em>");
        }
        table += tr;
    }
    for (const auto& x : heroes) {
        html_table_unit_row htr(x->second);
        table += htr.to_string();
    }
    table += "</table>";
    return table;
}

std::string army_list::html_units_table() const {
    const std::vector<std::string> headers = {
        "Unit", "Size", "Mount", "Weapons", "Armour", "Extras", "Command",
        "Banner", "Characteristics", "Points"
    };
    std::string table = "<table border=1 cellspacing=1 cellpadding=2 width=100%>\n"
                        "<thead><tr style=\"background-color: #000000\">\n"
                        "<th colspan=\"10\" style=\"color: #FFFFFF\">";
    table += "Core (" + tools::points_str(core_pts) + '/' + tools::points_str(core_min) + " points), ";
    table += "Special (" + tools::points_str(spec_pts) + '/' + tools::points_str(spec_lim) + " points) & ";
    table += "Rare (" + tools::points_str(rare_pts) + '/' + tools::points_str(rare_lim) + " points)";
    table += "</th></tr></thead>\n";
    // make field headers
    table += "<thead><tr style=\"background-color: #C0C0C0\">";
    for (const auto& h : headers) table += "<th style = \"color: #FFFFFF\">" + h + "</th\n>";
    table += "</tr></thead>\n";
    // fill table
    auto core = tools::find_all_if(std::begin(army), std::end(army), [](const auto& x) {
        return x.second->unit_type() == UnitType::CORE;
    });
    auto special = tools::find_all_if(std::begin(army), std::end(army), [](const auto& x) {
        return x.second->unit_type() == UnitType::SPECIAL;
    });
    auto rares = tools::find_all_if(std::begin(army), std::end(army), [](const auto& x) {
        return x.second->unit_type() == UnitType::RARE;
    });
    for (const auto& x : core) {
        html_table_unit_row htr(x->second);
        table += htr.to_string();
    }
    for (const auto& x : special) {
        html_table_unit_row htr(x->second);
        table += htr.to_string();
    }
    for (const auto& x : rares) {
        html_table_unit_row htr(x->second);
        table += htr.to_string();
    }
    table += "</table>";
    return table;
}

void army_list::save(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::runtime_error("Access error - unable to write to: " + filename.toStdString());
    QTextStream stream(&file);
    stream << "ARMY = " << QString::fromStdString(enum_convert::FACTION_TO_STRING.at(faction_)) << "\n\n";
    stream << "POINTS_LIMIT = " << QString::fromStdString(std::to_string(point_limit())) << "\n\n";
    for (const auto& u : army) stream << "UNIT = " << QString::fromStdString(u.second->save()) << "\n\n";
    if (general_ != nullptr)
        stream << "GENERAL = " << QString("%1").arg(general_->id()) << "\n\n";
}
