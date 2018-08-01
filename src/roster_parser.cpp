#include "roster_parser.h"
#include <iostream>
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
        if (s == "None" || s.empty()) return map;
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
            short level = static_cast<short>(std::stoi(v[0]));
            double pts = std::stod(tools::split(v[1], ']')[0]);
            um[level] = pts;
        }
        return um;
    }

    equipment roster_parser::parse_equipment(std::string s) {
        equipment e;
        if (s == "None" || s.empty()) {
            e.weapons[WeaponType::MELEE] = {ItemClass::MUNDANE, "Hand weapon"};
            return e;
        }
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
            else if (mount_bs[1] == "Monstrous Beast") uc = armies::UnitClass::MONSTROUS_BEAST;
            else if (mount_bs[1] == "Warbeast") uc = armies::UnitClass::WARBEASTS;
            else if (mount_bs[1] == "Chariot") uc = armies::UnitClass::CHARIOT;
            else if (mount_bs[1] == "Monster") uc = armies::UnitClass::MONSTER;
            else if (mount_bs[1] == "Unique") uc = armies::UnitClass::UNIQUE;
            double pts = std::stod(mount_bs[2]);
            um[name] = std::make_pair(uc, pts);
        }
        return um;
    }

    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > roster_parser::parse_optional_extras(std::string s) {
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto extras_bs = tools::parse_item_bs(_s);
            std::string name = extras_bs[0];
            bool is_singular = (extras_bs[1] == "True");
            double pts = std::stod(extras_bs[2]);
            um[name] = std::make_pair(is_singular, pts);
        }
        return um;
    }

    std::vector<std::shared_ptr<base_unit>> roster_parser::parse() {
        count_units();
        std::vector<std::shared_ptr<base_unit>> units;
        units.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string types = read_line(blocks[i] + 1);
            auto types_split = tools::split(types, ',');
            armies::UnitType ut = armies::s_map_string_unit_type[types_split[0]];
            switch (ut) {
            case armies::UnitType::LORD:
            case armies::UnitType::HERO:
                if (types_split[1] == "Melee")
                    units.push_back(std::make_shared<base_melee_character_unit>(
                        parse_melee_character(i, ut)
                    ));
                else if (types_split[1] == "Mage")
                    units.push_back(std::make_shared<base_mage_character_unit>(
                        parse_mage_character(i, ut)
                    ));
                break;
            case armies::UnitType::CORE:
            case armies::UnitType::SPECIAL:
            case armies::UnitType::RARE:
            {
                auto category = armies::s_map_string_unit_class[types_split[1]];
                if (types_split[2] == "Normal") {
                    switch (category) {
                    case armies::UnitClass::MONSTER:
                    case armies::UnitClass::SWARM:
                    case armies::UnitClass::MONSTROUS_BEAST:
                        units.push_back(
                                  std::make_shared<base_normal_unit>(
                                      parse_minimal_normal_unit(i, ut, category)
                                  )
                              );
                        break;
                    default:
                        units.push_back(std::make_shared<base_normal_unit>(
                            parse_normal_unit(i, ut, category)
                        ));
                        break;
                    }

                }
                else if (types_split[2] == "Mixed")
                    units.push_back(std::make_shared<base_mixed_unit>(
                        parse_mixed_unit(i, ut, category)
                    ));
                break;
            }
            default: continue;
            }
        }
        return units;
    }

    base_melee_character_unit roster_parser::parse_melee_character(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = armies::s_map_string_unit_class[read_line(blocks[n] + 2)];
        double pts = std::stod(read_line(blocks[n] + 3));
        //auto mm_size = parse_minmax_size(read_line(blocks[n] + 4));
        auto stats = tools::split_stos(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 7));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 8));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 9));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 10));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 11));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 12));
        options opt;
        opt.opt_weapons = opt_weapons;
        opt.opt_armour = opt_armours;
        opt.opt_mounts = opt_mounts;
        opt.oco_extras = opt_oco_extras;
        opt.mc_extras = opt_mc_extras;
        double mi_budget = std::stod(read_line(blocks[n] + 13));
        double fi_budget = std::stod(read_line(blocks[n] + 14));
        double ti_budget = std::stod(read_line(blocks[n] + 15));
        base_melee_character_unit tmp(
            faction,
            ut,
            category,
            name,
            pts,
            std::move(stats),
            std::move(rules),
            std::move(eq),
            std::move(opt),
            mi_budget,
            fi_budget,
            ti_budget,
            false // TODO: add is_bsb line to character block in roster file
        );
        return tmp;
    }

    base_mage_character_unit roster_parser::parse_mage_character(std::size_t n, armies::UnitType ut) {
        std::string name = read_line(blocks[n]);
        short level = static_cast<short>(std::stoi(read_line(blocks[n] + 2)));
        auto level_upgrades = parse_mage_upgrades(read_line(blocks[n] + 3));
        auto lores = tools::split(read_line(blocks[n] + 4), ',');
        auto category = armies::s_map_string_unit_class[read_line(blocks[n] + 5)];
        double pts = std::stod(read_line(blocks[n] + 6));
        //auto mm_size = parse_minmax_size(read_line(blocks[n] + 7));
        auto stats = tools::split_stos(read_line(blocks[n] + 8), ' ');
        auto rules = tools::split(read_line(blocks[n] + 9), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 10));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 11));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 12));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 13));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 14));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 15));
        options opt;
        opt.opt_weapons = opt_weapons;
        opt.opt_armour = opt_armours;
        opt.opt_mounts = opt_mounts;
        opt.oco_extras = opt_oco_extras;
        opt.mc_extras = opt_mc_extras;
        double mi_budget = std::stod(read_line(blocks[n] + 16));
        double fi_budget = std::stod(read_line(blocks[n] + 17));
        double ti_budget = std::stod(read_line(blocks[n] + 18));
        base_mage_character_unit tmp(
            faction,
            ut,
            category,
            name,
            pts,
            std::move(stats),
            std::move(rules),
            std::move(eq),
            std::move(opt),
            mi_budget,
            fi_budget,
            ti_budget,
            false,
            level,
            std::move(level_upgrades),
            std::move(lores)
        );
        return tmp;
    }

    base_normal_unit roster_parser::parse_normal_unit(
        std::size_t n, armies::UnitType ut, armies::UnitClass category, std::string name_, std::size_t offset
    ) {
        if (name_.empty())
            name_ = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2 + offset));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3 + offset));
        auto stats = tools::split_stos(read_line(blocks[n] + 4 + offset), ' ');
        auto champ_stats = tools::split_stos(read_line(blocks[n] + 5 + offset), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6 + offset), ',');
        auto champ_rules = tools::split(read_line(blocks[n] + 7 + offset), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 8 + offset));
        auto champ_eq = parse_equipment(read_line(blocks[n] + 9 + offset));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 10 + offset));
        auto champ_opt_weapons = parse_optional_weapons(read_line(blocks[n] + 11 + offset));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 12 + offset));
        auto champ_opt_armours = parse_optional_armour(read_line(blocks[n] + 13 + offset));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 14 + offset));
        auto champ_opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 15 + offset));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 16 + offset));
        auto champ_opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 17 + offset));
        options opt;
        opt.opt_weapons = opt_weapons;
        opt.opt_armour = opt_armours;
        opt.oco_extras = opt_oco_extras;
        opt.mc_extras = opt_mc_extras;
        options champ_opt;
        champ_opt.opt_weapons = champ_opt_weapons;
        champ_opt.opt_armour = champ_opt_armours;
        champ_opt.oco_extras = champ_opt_oco_extras;
        champ_opt.mc_extras = champ_opt_mc_extras;
        auto command = parse_command(read_line(blocks[n] + 18 + offset));
        double champ_mi_budget = std::stod(read_line(blocks[n] + 19 + offset));
        double champ_fi_budget = std::stod(read_line(blocks[n] + 20 + offset));
        double champ_ti_budget = std::stod(read_line(blocks[n] + 21 + offset));
        double mb_budget = std::stod(read_line(blocks[n] + 22 + offset));
        base_normal_unit tmp(
            faction,
            ut,
            category,
            name_,
            mm_size.first,
            mm_size.second,
            pts,
            std::move(stats),
            std::move(rules),
            std::move(eq),
            std::move(opt),
            std::move(champ_stats),
            std::move(champ_rules),
            std::move(champ_eq),
            std::move(champ_opt),
            champ_mi_budget,
            champ_fi_budget,
            champ_ti_budget,
            std::move(command),
            mb_budget
        );
        return tmp;
    }

    base_normal_unit roster_parser::parse_minimal_normal_unit(std::size_t n, armies::UnitType ut, armies::UnitClass category) {
        std::string name = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split_stos(read_line(blocks[n] + 4), ' ');
        std::vector<short> champ_stats;
        auto rules = tools::split(read_line(blocks[n] + 5), ',');
        std::vector<std::string> champ_rules;
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 6));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 7));
        equipment eq;
        equipment champ_eq;
        options opt;
        options champ_opt;
        opt.oco_extras = opt_oco_extras;
        opt.mc_extras = opt_mc_extras;
        std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        > command;
        base_normal_unit tmp(
            faction,
            ut,
            category,
            name,
            mm_size.first,
            mm_size.second,
            pts,
            std::move(stats),
            std::move(rules),
            std::move(eq),
            std::move(opt),
            std::move(champ_stats),
            std::move(champ_rules),
            std::move(champ_eq),
            std::move(champ_opt),
            0.0,
            0.0,
            0.0,
            std::move(command),
            0.0
        );
        return tmp;
    }

    base_mixed_unit roster_parser::parse_mixed_unit(std::size_t n, armies::UnitType ut, armies::UnitClass category) {
        auto slave = parse_normal_unit(n, ut, category);
        auto master = parse_normal_unit(n, ut, category, slave.name(), 21U);
        base_mixed_unit tmp(
            slave.name(),
            std::move(master),
            std::move(slave)
        );
        return tmp;
    }

}
