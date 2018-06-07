#include "selection_tree.h"

selection_tree::selection_tree(armies::Faction faction) : race(faction) {
    std::string filename;
    switch (race) {
    case armies::Faction::EMPIRE:
        filename = "empire_roster.ros";
        break;
    case armies::Faction::BRETONNIA:
        filename = "bretonnia_roster.ros";
        break;
    case armies::Faction::DWARFS:
        filename = "dwarfs_roster.ros";
        break;
    case armies::Faction::HIGH_ELVES:
        filename = "high_elves_roster.ros";
        break;
    case armies::Faction::WOOD_ELVES:
        filename = "wood_elves_roster.ros";
        break;
    case armies::Faction::DARK_ELVES:
        filename = "dark_elves_roster.ros";
        break;
    case armies::Faction::WARRIORS_OF_CHAOS:
        filename = "woc_roster.ros";
        break;
    case armies::Faction::DAEMONS_OF_CHAOS:
        filename = "doc_roster.ros";
        break;
    case armies::Faction::BEASTMEN:
        filename = "beastmen_roster.ros";
        break;
    case armies::Faction::CHAOS_DWARFS:
        filename = "chaos_dwarfs_roster.ros";
        break;
    case armies::Faction::VAMPIRE_COUNTS:
        filename = "vampire_counts_roster.ros";
        break;
    case armies::Faction::TOMB_KINGS:
        filename = "tomb_kings_roster.ros";
        break;
    case armies::Faction::OGRE_KINGDOMS:
        filename = "ogre_kingdoms_roster.ros";
        break;
    case armies::Faction::SKAVEN:
        filename = "skaven_roster.ros";
        break;
    case armies::Faction::ORCS_AND_GOBLINS:
        filename = "orcs_goblins_roster.ros";
        break;
    default:
        filename = "empire_roster.ros";
        break;
    }
    parse_roster_file(filename);
}

void selection_tree::change_selection(const std::string& name) {
    current_selection = std::make_shared<unit>(roster[name]);
}

void selection_tree::add_unit_to_army_list(army_list& list) {
    if (current_selection) list.add_unit(current_selection);
}

std::vector<
    std::pair<std::string, std::string>
> selection_tree::parse_table(const std::string& table) {
    std::vector<std::string> v = tools::split(table, ',');
    for (auto& s : v) tools::remove_leading_whitespaces(s);
    std::vector<std::pair<std::string, std::string>> pv;
    pv.resize(std::ceil(v.size()/2U));
    for (std::size_t i = 0U; i < pv.size(); ++i) {
        std::string s = v[2*i];
        (i % 2U) ? pv[i].second = s : pv[i].first = s;
    }
    return pv;
}

void selection_tree::parse_stat_table(unit& temp, const std::string& table) {
    std::vector<short> stat_vec = tools::split_stos(table, ' ');
    std::array<short, 9U> stats;
    for (std::size_t i = 0U; i < 9U; ++i) stats[i] = stat_vec[i];
    temp.set_stat_table(std::move(stats));
}

void selection_tree::parse_equipment_table(unit& temp, const std::string& table) {
    auto equipment = parse_table(table);
    temp.set_equipment(std::move(equipment));
}

void selection_tree::parse_special_rules_table(unit& temp, const std::string& table) {
    auto special_rules = parse_table(table);
    temp.set_special_rules(std::move(special_rules));
}

void selection_tree::parse_optional_weapons_table(unit& temp, const std::string& table) {

}

void selection_tree::parse_optional_armour_table(unit& temp, const std::string& table) {

}

void selection_tree::parse_optional_mounts_table(unit& temp, const std::string& table) {

}

void selection_tree::parse_optional_command_table(unit& temp, const std::string& table) {

}

void selection_tree::parse_optional_extra_table(unit& temp, const std::string& table) {

}

void selection_tree::parse_options_table(unit& temp, const std::string& table) {
    (void)temp;
    (void)table;
    // TODO: implement parse_options_table method
}

void selection_tree::parse_roster_file(const std::string& filename) {
    tools::file_reader fr("../rosters/" + filename);
    armies::Faction race = armies::s_map_string_faction[fr.read_line(0)];
    std::vector<std::size_t> block_positions;
    for (std::size_t i = 1U; i < fr.lines(); ++i) {
        std::string line = fr.read_line(i);
        if (tools::starts_with(line, '#') || line.empty()) continue;
        if (!(tools::starts_with(line, '    ') || tools::starts_with(line, '\t')))
            block_positions.push_back(i);
    }
    for (std::size_t i = 0U; i < block_positions.size(); ++i) {
        std::string unit_name = fr.read_line(block_positions[i]);
        armies::UnitType unit_type = armies::s_map_string_unit_type[
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 1]))
        ];
        std::size_t unit_pts = static_cast<std::size_t>(std::stoul(
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 2]))
        ));
        std::size_t unit_min_size = static_cast<std::size_t>(std::stoul(
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 3]))
        ));
        unit tmp(
            race,
            unit_type,
            unit_name,
            unit_pts,
            unit_min_size,
            unit_min_size
        );
        parse_stat_table(
            tmp,
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 4]))
        );
        parse_equipment_table(
            tmp,
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 5]))
        );
        parse_special_rules_table(
            tmp,
            tools::remove_leading_whitespaces(fr.read_line(block_positions[i + 6]))
        );
    }

    /*for (std::size_t i = 0U; i < fr.lines(); ++i) {
        std::string line = fr.read_line(i);
        if (tools::starts_with(line, '#')) continue;
        if (tools::starts_with(line, ':'))
            race = armies::s_map_string_faction[line.substr(1)];
        else {
            std::vector<std::string> split_line = tools::split(line, ';');
            if (split_line.empty()) continue;
            for (auto& s : split_line) tools::remove_leading_whitespaces(s);
            std::string unit_name = split_line[0];
            armies::UnitType unit_type = armies::s_map_string_unit_type[split_line[1]];
            std::size_t unit_pts = static_cast<std::size_t>(std::stoul(split_line[2]));
            std::size_t unit_min_size = static_cast<std::size_t>(std::stoul(split_line[3]));
            unit tmp(
                race,
                unit_type,
                unit_name,
                unit_pts,
                unit_min_size,
                unit_min_size
            );
            parse_stat_table(tmp, split_line[4]);
            parse_equipment_table(tmp, split_line[5]);
            parse_special_rules_table(tmp, split_line[6]);
            parse_options_table(tmp, split_line[7]);
            roster[split_line[0]] = tmp;
        }
    }*/
}
