#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(const QString& rfile, armies::Faction _faction)
        : f(rfile) {
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f);
        std::string content = in.readAll().toStdString();
        ss = std::stringstream(content);
        faction = _faction;
        cache();
    }

    roster_parser::~roster_parser() { f.close(); }

    void roster_parser::cache() {
        std::string s;
        while (ss) {
            streampos.push_back(ss.tellg());
            std::getline(ss, s);
        }
    }

    void roster_parser::count_units() {
        for (std::size_t i = 0U; i < streampos.size(); ++i) {
            std::string line = read_line(i, false);
            if (line.empty() || tools::starts_with(line, '#')) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void roster_parser::navigate_to_line(std::size_t n) {
        ss.clear();
        ss.seekg(streampos[n]);
    }

    std::string roster_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(ss, s);
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

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > roster_parser::parse_command(std::string s) {
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

    std::unordered_map<
        short,
        double
    > roster_parser::parse_mage_upgrades(std::string s) {
        std::unordered_map<short, double> um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> v = tools::split(_s, '[');
            short level = std::stoul(v[0]);
            double pts = std::stod(tools::split(v[1], ']')[0]);
            um[level] = pts;
        }
        return um;
    }

    equipment roster_parser::parse_equipment(std::string s) {
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
                ItemClass ic;
                if (item_bs[1] == "Mundane") ic = ItemClass::MUNDANE;
                else if (item_bs[1] == "Magic") ic = ItemClass::MAGIC;
                else if (item_bs[1] == "Common") ic = ItemClass::COMMON;
                else if (item_bs[1] == "Faction") ic = ItemClass::FACTION;
                WeaponType wt;
                if (item_bs[2] == "Melee") wt = WeaponType::MELEE;
                else if (item_bs[2] == "Ballistic") wt = WeaponType::BALLISTIC;
                e.weapons[wt] = {ic, name};
                break;
            }
            case 1:
            {
                auto item_bs = tools::parse_item_bs(vec[1]);
                std::string name = item_bs[0];
                ItemClass ic;
                if (item_bs[1] == "Mundane") ic = ItemClass::MUNDANE;
                else if (item_bs[1] == "Magic") ic = ItemClass::MAGIC;
                else if (item_bs[1] == "Common") ic = ItemClass::COMMON;
                else if (item_bs[1] == "Faction") ic = ItemClass::FACTION;
                ArmourType at;
                if (item_bs[2] == "Armour") at = ArmourType::ARMOUR;
                else if (item_bs[2] == "Shield") at = ArmourType::SHIELD;
                else if (item_bs[2] == "Helmet") at = ArmourType::HELMET;
                e.armour[at] = {ic, name};
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
        if (!(e.weapons.count(WeaponType::MELEE)))
            e.weapons[WeaponType::MELEE] = {ItemClass::MUNDANE, "Hand weapon"};
        return e;
    }

    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > roster_parser::parse_optional_weapons(std::string s) {
        std::unordered_map<
            std::string,
            std::tuple<WeaponType, ItemClass, double>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto weapon_bsp = tools::parse_item_bsp(_s);
            std::string name = weapon_bsp[0];
            ItemClass ic;
            if (weapon_bsp[1] == "Mundane") ic = ItemClass::MUNDANE;
            else if (weapon_bsp[1] == "Magic") ic = ItemClass::MAGIC;
            else if (weapon_bsp[1] == "Common") ic = ItemClass::COMMON;
            else if (weapon_bsp[1] == "Faction") ic = ItemClass::FACTION;
            WeaponType wt;
            if (weapon_bsp[2] == "Melee") wt = WeaponType::MELEE;
            else if (weapon_bsp[2] == "Ballistic") wt = WeaponType::BALLISTIC;
            double pts = std::stod(weapon_bsp[3]);
            //um[name] = std::make_pair(wt, pts);
            um[name] = std::make_tuple(wt, ic, pts);
        }
        return um;
    }

    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > roster_parser::parse_optional_armour(std::string s) {
        std::unordered_map<
            std::string,
            std::tuple<ArmourType, ItemClass, double>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto armour_bsp = tools::parse_item_bsp(_s);
            std::string name = armour_bsp[0];
            ItemClass ic;
            if (armour_bsp[1] == "Mundane") ic = ItemClass::MUNDANE;
            else if (armour_bsp[1] == "Magic") ic = ItemClass::MAGIC;
            else if (armour_bsp[1] == "Common") ic = ItemClass::COMMON;
            else if (armour_bsp[1] == "Faction") ic = ItemClass::FACTION;
            ArmourType at;
            if (armour_bsp[2] == "Armour") at = ArmourType::ARMOUR;
            else if (armour_bsp[2] == "Shield") at = ArmourType::SHIELD;
            else if (armour_bsp[2] == "Helmet") at = ArmourType::HELMET;
            double pts = std::stod(armour_bsp[3]);
            //um[name] = std::make_pair(at, pts);
            um[name] = std::make_tuple(at, ic, pts);
        }
        return um;
    }

    std::unordered_map<
        std::string,
        std::pair<armies::UnitClass, double>
    > roster_parser::parse_optional_mounts(std::string s) {
        std::unordered_map<
            std::string,
            std::pair<armies::UnitClass, double>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto mount_bs = tools::parse_item_bs(_s);
            std::string name = mount_bs[0];
            armies::UnitClass uc;
            if (mount_bs[1] == "Infantry") uc = armies::UnitClass::INFANTRY;
            else if (mount_bs[1] == "Cavalry") uc = armies::UnitClass::CAVALRY;
            else if (mount_bs[1] == "Monstrous Cavalry") uc = armies::UnitClass::MONSTROUS_CAVALRY;
            else if (mount_bs[1] == "Monstrous Creature") uc = armies::UnitClass::MONSTROUS_CREATURES;
            else if (mount_bs[1] == "Warbeast") uc = armies::UnitClass::WARBEASTS;
            else if (mount_bs[1] == "Chariot") uc = armies::UnitClass::CHARIOT;
            else if (mount_bs[1] == "Monster") uc = armies::UnitClass::MONSTER;
            else if (mount_bs[1] == "Unique") uc = armies::UnitClass::UNIQUE;
            double pts = std::stod(mount_bs[2]);
            um[name] = std::make_pair(uc, pts);
        }
        return um;
    }

    std::pair<
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >,
        bool
    > roster_parser::parse_optional_extras(std::string s) {
        std::pair<
            std::unordered_map<
                std::string,
                std::pair<bool, double>
            >,
            bool
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all;
        if (tools::starts_with(s, "OCO")) {
            um.second = true;
            all = tools::split(s.substr(4), ',');
        }
        else all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto extras_bs = tools::parse_item_bs(_s);
            std::string name = extras_bs[0];
            bool is_singular = (extras_bs[1] == "True");
            double pts = std::stod(extras_bs[2]);
            (um.first)[name] = std::make_pair(is_singular, pts);
        }
        return um;
    }

    std::vector<base_unit> roster_parser::parse() {
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
            case armies::UnitType::CORE:
            case armies::UnitType::SPECIAL:
            case armies::UnitType::RARE:
                if (types_split[1] == "Infantry")
                    units.push_back(parse_infantry(i, ut));
                else if (types_split[1] == "Swarm")
                    units.push_back(parse_swarm(i, ut));
                else if (types_split[1] == "Monster")
                    units.push_back(parse_monster(i, ut));
                else if (types_split[1] == "Warmachine")
                    units.push_back(parse_warmachine(i, ut));
            default: continue;
            }
        }
        return units;
    }

    base_unit roster_parser::parse_melee_character(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::s_map_string_unit_class[read_line(blocks[n] + 2)];
        double pts = std::stod(read_line(blocks[n] + 3));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 4));
        auto stats = tools::split_stos(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 7));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 8));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 9));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 10));
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 11));
        double mi_budget = std::stod(read_line(blocks[n] + 12));
        double fi_budget = std::stod(read_line(blocks[n] + 13));
        double ti_budget = std::stod(read_line(blocks[n] + 14));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = true;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.special_rules = rules;
        tmp.eq = eq;
        tmp.opt_weapons = opt_weapons;
        tmp.opt_armour = opt_armours;
        tmp.opt_mounts = opt_mounts;
        tmp.opt_extras = opt_extras;
        tmp.magic_item_budget = mi_budget;
        tmp.faction_item_budget = fi_budget;
        tmp.total_item_budget = ti_budget;
        return tmp;
    }

    base_unit roster_parser::parse_mage_character(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        short level = static_cast<short>(std::stoi(read_line(blocks[n] + 2)));
        auto level_upgrades = parse_mage_upgrades(read_line(blocks[n] + 3));
        auto lores = tools::split(read_line(blocks[n] + 4), ',');
        auto category = armies::s_map_string_unit_class[read_line(blocks[n] + 5)];
        double pts = std::stod(read_line(blocks[n] + 6));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 7));
        auto stats = tools::split_stos(read_line(blocks[n] + 8), ' ');
        auto rules = tools::split(read_line(blocks[n] + 9), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 10));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 11));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 12));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 13));
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 14));
        double mi_budget = std::stod(read_line(blocks[n] + 15));
        double fi_budget = std::stod(read_line(blocks[n] + 16));
        double ti_budget = std::stod(read_line(blocks[n] + 17));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = true;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.special_rules = rules;
        tmp.eq = eq;
        tmp.opt_weapons = opt_weapons;
        tmp.opt_armour = opt_armours;
        tmp.opt_mounts = opt_mounts;
        tmp.opt_extras = opt_extras;
        tmp.magic_item_budget = mi_budget;
        tmp.faction_item_budget = fi_budget;
        tmp.total_item_budget = ti_budget;
        tmp.base_mage_level = level;
        tmp.mage_level_upgrades = level_upgrades;
        tmp.lores = lores;
        return tmp;
    }

    base_unit roster_parser::parse_infantry(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::UnitClass::INFANTRY;
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split_stos(read_line(blocks[n] + 4), ' ');
        auto champ_stats = tools::split_stos(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 7));
        auto champ_eq = parse_equipment(read_line(blocks[n] + 8));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 9));
        auto champ_opt_weapons = parse_optional_weapons(read_line(blocks[n] + 10));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 11));
        auto champ_opt_armours = parse_optional_armour(read_line(blocks[n] + 12));
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 13));
        auto champ_opt_extras = parse_optional_extras(read_line(blocks[n] + 14));
        auto command = parse_command(read_line(blocks[n] + 15));
        double champ_mi_budget = std::stod(read_line(blocks[n] + 16));
        double champ_fi_budget = std::stod(read_line(blocks[n] + 17));
        double champ_ti_budget = std::stod(read_line(blocks[n] + 18));
        double mb_budget = std::stod(read_line(blocks[n] + 19));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = false;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.champion_stats = champ_stats;
        tmp.special_rules = rules;
        tmp.eq = eq;
        tmp.champ_eq = champ_eq;
        tmp.opt_weapons = opt_weapons;
        tmp.opt_armour = opt_armours;
        tmp.opt_extras = opt_extras;
        tmp.opt_command = command;
        tmp.champ_opt_weapons = champ_opt_weapons;
        tmp.champ_opt_armour = champ_opt_armours;
        tmp.champ_opt_extras = champ_opt_extras;
        tmp.magic_item_budget = champ_mi_budget;
        tmp.faction_item_budget = champ_fi_budget;
        tmp.total_item_budget = champ_ti_budget;
        tmp.magic_banner_budget = mb_budget;
        return tmp;
    }

    base_unit roster_parser::parse_swarm(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::UnitClass::SWARM;
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split_stos(read_line(blocks[n] + 4), ' ');
        auto rules = tools::split(read_line(blocks[n] + 5), ',');
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 6));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = false;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.special_rules = rules;
        tmp.opt_extras = opt_extras;
        tmp.magic_item_budget = 0.0;
        tmp.faction_item_budget = 0.0;
        tmp.total_item_budget = 0.0;
        tmp.magic_banner_budget = 0.0;
        return tmp;
    }

    base_unit roster_parser::parse_monster(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::UnitClass::MONSTER;
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split_stos(read_line(blocks[n] + 4), ' ');
        auto rules = tools::split(read_line(blocks[n] + 5), ',');
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 6));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = false;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.special_rules = rules;
        tmp.opt_extras = opt_extras;
        tmp.magic_item_budget = 0.0;
        tmp.faction_item_budget = 0.0;
        tmp.total_item_budget = 0.0;
        tmp.magic_banner_budget = 0.0;
        return tmp;
    }

    base_unit roster_parser::parse_warmachine(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::UnitClass::WARMACHINE;
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split_stos(read_line(blocks[n] + 4), ' ');
        auto crew_stats = tools::split_stos(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto opt_extras = parse_optional_extras(read_line(blocks[n] + 7));
        base_unit tmp;
        tmp.faction = faction;
        tmp.unit_type = ut;
        tmp.unit_class = category;
        tmp.name = name;
        tmp.is_character = false;
        tmp.is_unique = false;
        tmp.is_mage = false;
        tmp.pts_per_model = pts;
        tmp.min_size = mm_size.first;
        tmp.max_size = mm_size.second;
        tmp.stats = stats;
        tmp.champion_stats = crew_stats;
        tmp.special_rules = rules;
        tmp.opt_extras = opt_extras;
        tmp.magic_item_budget = 0.0;
        tmp.faction_item_budget = 0.0;
        tmp.total_item_budget = 0.0;
        tmp.magic_banner_budget = 0.0;
        return tmp;
    }

}
