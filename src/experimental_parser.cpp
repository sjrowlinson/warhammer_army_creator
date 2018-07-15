#include "experimental_parser.h"

namespace tools {

    experimental_parser::experimental_parser(const QString& rfile, armies::Faction _faction)
        : f(rfile) {
        f.open(QFile::ReadOnly | QFile::Text);
        qts = QTextStream(&f);
        faction = _faction;
        cache();
    }

    experimental_parser::~experimental_parser() { f.close(); }

    void experimental_parser::cache() {
        while (!qts.atEnd()) {
            streampos.push_back(qts.pos());
            auto line = qts.readLine();
        }
    }

    void experimental_parser::count_units() {
        for (std::size_t i = 0U; i < spos.size(); ++i) {
            std::string line = read_line(i, false);
            if (line.empty() || tools::starts_with(line, '#')) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void experimental_parser::navigate_to_line(std::size_t n) {
        qts.seek(streampos[n]);
    }

    std::string experimental_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string line = qts.readLine().toStdString();
        if (trim) return tools::remove_leading_whitespaces(line);
        return line;
    }

    std::pair<std::size_t, std::size_t> experimental_parser::parse_minmax_size(std::string s) {
        std::vector<std::string> min_max_str = tools::split(s, ',');
        std::size_t min = 1;
        std::size_t max = std::numeric_limits<std::size_t>::max();
        if (min_max_str[1] != "None") max = static_cast<std::size_t>(std::stoul(min_max_str[1]));
        min = static_cast<std::size_t>(std::stoul(min_max_str[0]));
        return std::make_pair(min, max);
    }

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > experimental_parser::parse_command(std::string s) {
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
                tools::remove_trailing_whitespaces(name_pts[0]), pts
            );
        }
        return map;
    }

    experimental_parser::equipment experimental_parser::parse_equipment(std::string s) {
        equipment e;
        if (s == "None" || s.empty()) return e;
        std::unordered_map<std::string, int> map_to = {
            {"Weapon", 0},
            {"Armour", 1},
            {"Talisman", 2},
            {"Arcane Item", 3},
            {"Enchanted Item", 4},
            {"Banner", 5}
        };
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> vec = tools::split(_s, ':');
            switch (map_to[vec[0]]) {
            case 0:
            {
                auto item_bs = tools::parse_item_bs(vec[1]);
                std::string name = item_bs[0];
                WeaponType wt;
                if (item_bs[2] == "Melee") wt = WeaponType::MELEE;
                else if (item_bs[2] == "Ballistic") wt = WeaponType::BALLISTIC;
                e.weapons[wt] = name;
                break;
            }
            case 1:
            {
                auto item_bs = tools::parse_item_bs(vec[1]);
                std::string name = item_bs[0];
                ArmourType at;
                if (item_bs[2] == "Armour") at = ArmourType::ARMOUR;
                else if (item_bs[2] == "Shield") at = ArmourType::SHIELD;
                else if (item_bs[2] == "Helmet") at = ArmourType::HELMET;
                e.armour[at] = name;
                break;
            }
                break;
            case 2:
                e.talismans.push_back(vec[1]);
                break;
            case 3:
                e.arcane.push_back(vec[1]);
                break;
            case 4:
                e.enchanted.push_back(vec[1]);
                break;
            case 5:
                e.banners.push_back(vec[1]);
                break;
            default: break;
            }
        }
        return e;
    }

    std::unordered_map<
        std::string,
        std::vector<std::pair<WeaponType, double>>
    > experimental_parser::parse_optional_weapons(std::string s) {
        std::unordered_map<
                std::string,
                std::vector<std::pair<WeaponType, double>>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto weapon_bsp = tools::parse_item_bsp(_s);
            std::string name = weapon_bsp[0];
            ItemClass ic;
            if (bsp[1] == "Mundane") ic = ItemClass::MUNDANE;
            else if (bsp[1] == "Magic") ic = ItemClass::MAGIC;
            else if (bsp[1] == "Common") ic = ItemClass::COMMON;
            else if (bsp[1] == "Faction") ic = ItemClass::FACTION;
            WeaponType wt;
            if (bsp[2] == "Melee") wt = WeaponType::MELEE;
            else if (bsp[2] == "Ballistic") wt = WeaponType::BALLISTIC;
            double pts = std::stod(bsp[3]);
            um[name].push_back(std::make_pair(wt, pts));
        }
        return um;
    }

    std::unordered_map<
        std::string,
        std::vector<std::pair<ArmourType, double>>
    > experimental_parser::parse_optional_armour(std::string s) {
        std::unordered_map<
                std::string,
                std::vector<std::pair<ArmourType, double>>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto armour_bsp = tools::parse_item_bsp(_s);
            std::string name = armour_bsp[0];
            ItemClass ic;
            if (bsp[1] == "Mundane") ic = ItemClass::MUNDANE;
            else if (bsp[1] == "Magic") ic = ItemClass::MAGIC;
            else if (bsp[1] == "Common") ic = ItemClass::COMMON;
            else if (bsp[1] == "Faction") ic = ItemClass::FACTION;
            ArmourType at;
            if (bsp[2] == "Armour") at = ArmourType::ARMOUR;
            else if (bsp[2] == "Shield") at = ArmourType::SHIELD;
            else if (bsp[2] == "Helmet") at = ArmourType::HELMET;
            double pts = std::stod(bsp[3]);
            um[name].push_back(std::make_pair(at, pts));
        }
        return um;
    }

    std::vector<base_unit> experimental_parser::parse() {
        count_units();
        std::vector<base_unit> units;
        units.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string types = read_line(blocks[i] + 1);
            auto types_split = tools::split(types, ',');
            armies::UnitType ut = armies::s_map_string_unit_type[types_split[0]];
            switch (ut) {
            case armies::UnitType::LORD:
            case armies::UnitType::HERO:
                if (types_split[1] == "Melee")
                    units.push_back(parse_melee_character(i, ut));
                else if (types_split[1] == "Mage")
                    units.push_back(parse_mage_character(i, ut));
                break;
            default: continue;
            }
        }
        return units;
    }

    base_unit experimental_parser::parse_melee_character(std::size_t n, armies::UnitType ut) {
        auto name = read_line(blocks[n]);
        auto category = armies::s_map_string_unit_class[read_line(blocks[n] + 2)];
        auto pts = std::stod(read_line(blocks[n] + 3));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 4));
        auto stats = tools::split_stos(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 7));
    }

}






