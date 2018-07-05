#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(
        const std::string& name,
        armies::Faction faction
    ) : fs(name), filename(name), streampos(), blocks(), faction(faction) {
        cache_streampos();
    }

    void roster_parser::cache_streampos() {
        std::string s;
        while (fs) {
            streampos.push_back(fs.tellg());
            std::getline(fs, s);
        }
    }

    void roster_parser::count_units() {
        for (std::size_t i = 0U; i < streampos.size(); ++i) {
            std::string line = read_line(i, false);
            if (tools::starts_with(line, '#') || line.empty()) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void roster_parser::navigate_to_line(std::size_t n) {
        fs.clear();
        fs.seekg(streampos[n]);
    }

    std::string roster_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(fs, s);
        if (trim) return tools::remove_leading_whitespaces(s);
        return s;
    }

	std::pair<std::size_t, std::size_t> roster_parser::parse_minmax_size(std::string s) {
        std::vector<std::string> min_max_str = tools::split(s, ',');
        std::size_t min = 1;
        std::size_t max = std::numeric_limits<std::size_t>::max();
        if (min_max_str[1] != "None") max = static_cast<std::size_t>(std::stoul(min_max_str[1]));
        min = static_cast<std::size_t>(std::stoul(min_max_str[0]));
        return std::make_pair(min, max);
    }

    std::vector<std::pair<std::string, double>> roster_parser::common_option_parse(std::string s) {
        std::vector<std::pair<std::string, double>> vec;
        if (s == "None" || s.empty()) return vec;
        // first parse to vec of strings
        std::vector<std::string> svec = tools::split(s, ',');
        for (const auto& _s : svec) {
            std::vector<std::string> tmp = tools::split(_s, '[');
            std::string pts_str;
            std::copy_if(
                tmp[1].begin(), tmp[1].end(), std::back_inserter(pts_str), [](auto x) { return x != ']'; }
            );
            double pts = std::stod(pts_str);
            vec.push_back(
                std::make_pair(tmp[0].substr(0, tmp[0].size() - 1U), pts)
            );
        }
        return vec;
    }

	std::unordered_map<
		CommandGroup, std::pair<std::string, double>
	> roster_parser::parse_command_group(std::string s) {
        std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        > map;
        std::unordered_map<std::string, CommandGroup> convert = {
            {"Musician", CommandGroup::MUSICIAN},
            {"Standard Bearer", CommandGroup::STANDARD_BEARER},
            {"Champion", CommandGroup::CHAMPION}
        };
        std::vector<std::string> entries = tools::split(s, ',');
        for (const auto& x : entries) {
            std::vector<std::string> tmp = tools::split(x, ':');
            std::vector<std::string> name_pts = tools::split(tmp[1], '[');
            std::string pts_str;
            std::copy_if(
                name_pts[1].begin(), name_pts[1].end(), std::back_inserter(pts_str), [](auto x) { return x != ']'; }
            );
            double pts = std::stod(pts_str);
            map[convert[tmp[0]]] = std::make_pair(
                name_pts[0].substr(0, name_pts[0].size() - 1U), pts
            );
        }
        return map;
    }

    std::vector<base_unit> roster_parser::parse() {
        count_units();
        std::vector<base_unit> units; units.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string types = read_line(blocks[i]+1);
            std::vector<std::string> types_split = tools::split(types, ',');
            for (auto& s : types_split) tools::remove_leading_whitespaces(s);
            armies::UnitType unit_type = armies::s_map_string_unit_type[types_split[0]];
            switch (unit_type) {
            case armies::UnitType::LORD:
            case armies::UnitType::HERO:
                if (types_split[1] == "Melee") units.push_back(parse_melee_character(i, unit_type));
                else if (types_split[1] == "Mage") units.push_back(parse_mage_character(i, unit_type));
                break;
            case armies::UnitType::CORE:
            case armies::UnitType::SPECIAL:
            case armies::UnitType::RARE:
                if (types_split[1] == "Infantry") units.push_back(parse_infantry(i, unit_type));
                else if (types_split[1] == "Cavalry") parse_cavalry(i, unit_type);
                else if (types_split[1] == "Warbeast") parse_warbeast(i, unit_type);
                else if (types_split[1] == "Monstrous Creature") parse_monstrous_creature(i, unit_type);
                else if (types_split[1] == "Monster") parse_monster(i, unit_type);
                break;
            default: continue;
            }
        }
        return units;
    }

    // TODO: make common parse_unit function once all the individual ones below are worked
    // out - lots of duplication at the moment which can be reduced

	base_unit roster_parser::parse_melee_character(std::size_t index, armies::UnitType unit_type) {
        std::string name = read_line(blocks[index]);
        armies::UnitClass category = armies::s_map_string_unit_class[
            read_line(blocks[index] + 2)
        ];
        double pts_per_model = std::stod(read_line(blocks[index] + 3));
        auto minmax_size = parse_minmax_size(read_line(blocks[index] + 4));
        std::vector<short> stats = tools::split_stos(read_line(blocks[index] + 5), ' ');
        std::vector<std::string> equipment = tools::split(read_line(blocks[index] + 6), ',');
        std::vector<std::string> special_rules = tools::split(read_line(blocks[index] + 7), ',');
        std::vector<
            std::pair<std::string, double>
        > optional_weapons = common_option_parse(read_line(blocks[index] + 8));
        std::vector<
            std::pair<std::string, double>
        > optional_armour = common_option_parse(read_line(blocks[index] + 9));
        std::vector<
            std::pair<std::string, double>
        > optional_mounts = common_option_parse(read_line(blocks[index] + 10));
        double magic_item_allowance = std::stod(read_line(blocks[index] + 11));
        std::vector<std::string> str_extra_allowance = tools::split(read_line(blocks[index] + 12), ',');
        double extra_item_allowance = std::stod(str_extra_allowance[0]);
        std::vector<
            std::pair<std::string, double>
        > extras = common_option_parse(read_line(blocks[index] + 13));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = unit_type;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = true;
        tmp.is_unique = false; // TODO: implement
        tmp.pts_per_model = pts_per_model;
        tmp.min_size = minmax_size.first;
        tmp.max_size = minmax_size.second;
        tmp.stats = stats;
        tmp.equipment = equipment;
        tmp.special_rules = special_rules;
        for (auto&& x : optional_weapons) tmp.opt_weapons[x.first] = x.second;
        for (auto&& x : optional_armour) tmp.opt_armour[x.first] = x.second;
        for (auto&& x : optional_mounts) tmp.opt_mounts[x.first] = x.second;
        for (auto&& x : extras) tmp.opt_extras[x.first] = x.second;
        tmp.magic_item_budget = magic_item_allowance;
        tmp.faction_item_budget = extra_item_allowance;
        return tmp;
    }

    base_unit roster_parser::parse_mage_character(std::size_t index, armies::UnitType unit_type) {
        std::string name = read_line(blocks[index]);
        short mage_level = static_cast<short>(std::stoi(read_line(blocks[index] + 2)));
        std::string mage_upgrades_str = read_line(blocks[index] + 3);
        std::vector<std::pair<short, double>> mage_upgrades;
        if (mage_upgrades_str != "None") {
            std::vector<std::string> _tmp = tools::split(mage_upgrades_str, ',');
            for (const auto& _s : _tmp) {
                std::vector<std::string> _tmp1 = tools::split(_s, '[');
                std::string pts_str;
                std::copy_if(
                    _tmp1[1].begin(), _tmp1[1].end(), pts_str.begin(), [](auto x) {
                        return x != ']';
                    }
                );
                double pts = std::stod(pts_str);
                mage_upgrades.push_back(
                    std::make_pair(static_cast<short>(std::stoi(_tmp1[0])), pts)
                );
            }
        }
        std::string magic_lores_str = read_line(blocks[index] + 4);
        std::vector<std::string> lores;
        if (magic_lores_str != "None")
            lores = tools::split(magic_lores_str, ',');
        armies::UnitClass category = armies::s_map_string_unit_class[
            read_line(blocks[index] + 5)
        ];
        double pts_per_model = std::stod(read_line(blocks[index] + 6));
        auto minmax_size = parse_minmax_size(read_line(blocks[index] + 7));
        std::vector<short> stats = tools::split_stos(read_line(blocks[index] + 8), ' ');
        std::string equipment_str = read_line(blocks[index] + 9);
        std::vector<std::string> equipment;
        if (equipment_str != "None")
            equipment = tools::split(equipment_str, ',');
        std::string special_rules_str = read_line(blocks[index] + 10);
        std::vector<std::string> special_rules;
        if (special_rules_str != "None")
            special_rules = tools::split(special_rules_str, ',');
        std::vector<
            std::pair<std::string, double>
        > optional_weapons = common_option_parse(read_line(blocks[index] + 11));
        std::vector<
            std::pair<std::string, double>
        > optional_armour = common_option_parse(read_line(blocks[index] + 12));
        std::vector<
            std::pair<std::string, double>
        > optional_mounts = common_option_parse(read_line(blocks[index] + 13));
        double magic_item_allowance = std::stoul(read_line(blocks[index] + 14));
        std::vector<std::string> extra_allowance_str = tools::split(read_line(blocks[index] + 15), ',');
        double extra_item_allowance = std::stoul(extra_allowance_str[0]);
        std::vector<
            std::pair<std::string, double>
        > extras = common_option_parse(read_line(blocks[index] + 16));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = unit_type;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = true;
        tmp.is_unique = false; // TODO: implement
        tmp.pts_per_model = pts_per_model;
        tmp.min_size = minmax_size.first;
        tmp.max_size = minmax_size.second;
        tmp.stats = stats;
        tmp.equipment = equipment;
        tmp.special_rules = special_rules;
        for (auto&& x : optional_weapons) tmp.opt_weapons[x.first] = x.second;
        for (auto&& x : optional_armour) tmp.opt_armour[x.first] = x.second;
        for (auto&& x : optional_mounts) tmp.opt_mounts[x.first] = x.second;
        for (auto&& x : extras) tmp.opt_extras[x.first] = x.second;
        tmp.magic_item_budget = magic_item_allowance;
        tmp.faction_item_budget = extra_item_allowance;
        tmp.base_mage_level = mage_level;
        for (auto&& x : mage_upgrades) tmp.mage_level_upgrades[x.first] = x.second;
        tmp.lores = lores;
        return tmp;
    }

    base_unit roster_parser::parse_infantry(std::size_t index, armies::UnitType unit_type) {
        // get the unit name
        std::string name = read_line(blocks[index]);
        armies::UnitClass category = armies::UnitClass::INFANTRY;
        double pts_per_model = std::stod(read_line(blocks[index] + 2));
        auto minmax_size = parse_minmax_size(read_line(blocks[index] + 3));
        // statistics tables of the unit (champion treated separately)
        std::vector<short> stats = tools::split_stos(read_line(blocks[index] + 4), ' ');
        std::vector<short> champion_stats =
            tools::split_stos(read_line(blocks[index] + 5), ' ');
        // parse unit equipment and special rules
        std::string equipment_str = read_line(blocks[index] + 6);
        std::vector<std::string> equipment;
        if (equipment_str != "None")
            equipment = tools::split(equipment_str, ',');
        std::string special_rules_str = read_line(blocks[index] + 7);
        std::vector<std::string> special_rules;
        if (special_rules_str != "None")
            special_rules = tools::split(special_rules_str, ',');
        // parse the available equipment options for the unit
        std::vector<
            std::pair<std::string, double>
        > optional_weapons = common_option_parse(read_line(blocks[index] + 8));
        std::vector<
            std::pair<std::string, double>
        > optional_armour = common_option_parse(read_line(blocks[index] + 9));
        // get the optional command characters
        auto optional_command = parse_command_group(read_line(blocks[index] + 10));
        // item budgets for the champion and standard bearer
        double champion_magic_item_budget = std::stod(read_line(blocks[index] + 11));
        double champion_extra_item_budget = std::stod(read_line(blocks[index] + 12));
        double magic_banner_budget = std::stod(read_line(blocks[index] + 13));
        // any unique extras such as weapons teams for Skaven or detachments for The Empire
        std::vector<
            std::pair<std::string, double>
        > extras = common_option_parse(read_line(blocks[index] + 14));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = unit_type;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = true;
        tmp.is_unique = false; // TODO: implement
        tmp.pts_per_model = pts_per_model;
        tmp.min_size = minmax_size.first;
        tmp.max_size = minmax_size.second;
        tmp.stats = stats;
        tmp.equipment = equipment;
        tmp.special_rules = special_rules;
        for (auto&& x : optional_weapons) tmp.opt_weapons[x.first] = x.second;
        for (auto&& x : optional_armour) tmp.opt_armour[x.first] = x.second;
        for (auto&& x : extras) tmp.opt_extras[x.first] = x.second;
        tmp.opt_command = optional_command;
        tmp.magic_item_budget = champion_magic_item_budget;
        tmp.faction_item_budget = champion_extra_item_budget;
        tmp.magic_banner_budget = magic_banner_budget;
        return tmp;
    }

	base_unit roster_parser::parse_cavalry(std::size_t index, armies::UnitType unit_type) {

    }

	base_unit roster_parser::parse_warbeast(std::size_t index, armies::UnitType unit_type) {

    }

	base_unit roster_parser::parse_monstrous_creature(std::size_t index, armies::UnitType unit_type) {

    }

	base_unit roster_parser::parse_monster(std::size_t index, armies::UnitType unit_type) {

    }

}
