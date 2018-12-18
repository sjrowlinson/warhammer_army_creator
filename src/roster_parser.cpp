#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(const QString& rfile, Faction _faction)
        : file_parser(rfile), faction(_faction), tpo() {
        find_blocks();
    }

    void roster_parser::register_bindings() {
        using std::placeholders::_1;
        parsing_functions["TYPE"] =
            std::bind(&roster_parser::parse_unit_type, *this, _1, false, true);
        parsing_functions["CATEGORY"] =
            std::bind(&roster_parser::parse_unit_category, *this, _1, false, true);
        parsing_functions["MOUNT"] =
            std::bind(&roster_parser::parse_unit_mount, *this, _1, false, true);
        parsing_functions["POINTS"] =
            std::bind(&roster_parser::parse_unit_points, *this, _1, false, true);
        parsing_functions["SIZE"] =
            std::bind(&roster_parser::parse_unit_size, *this, _1, false, true);
        parsing_functions["MAGE_LEVEL"] =
            std::bind(&roster_parser::parse_unit_mage_level, *this, _1, false, true);
        parsing_functions["MAGE_UPGRADES"] =
            std::bind(&roster_parser::parse_unit_mage_upgrades, *this, _1, false, true);
        parsing_functions["CHARACTERISTICS"] =
            std::bind(&roster_parser::parse_unit_characteristics, *this, _1, false, true);
        parsing_functions["CHAMPION_CHARACTERISTICS"] =
            std::bind(&roster_parser::parse_unit_characteristics, *this, _1, true, true);
        parsing_functions["SPECIAL_RULES"] =
            std::bind(&roster_parser::parse_unit_special_rules, *this, _1, false, true);
        parsing_functions["CHAMPION_SPECIAL_RULES"] =
            std::bind(&roster_parser::parse_unit_special_rules, *this, _1, true, true);
        parsing_functions["EQUIPMENT"] =
            std::bind(&roster_parser::parse_unit_equipment, *this, _1, false, true);
        parsing_functions["CHAMPION_EQUIPMENT"] =
            std::bind(&roster_parser::parse_unit_equipment, *this, _1, true, true);
        parsing_functions["OPTIONAL_WEAPONS"] =
            std::bind(&roster_parser::parse_unit_optional_weapons, *this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_WEAPONS"] =
            std::bind(&roster_parser::parse_unit_optional_weapons, *this, _1, true, true);
        parsing_functions["OPTIONAL_ARMOUR"] =
            std::bind(&roster_parser::parse_unit_optional_armour, *this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_ARMOUR"] =
            std::bind(&roster_parser::parse_unit_optional_armour, *this, _1, true, true);
        parsing_functions["OPTIONAL_MOUNTS"] =
            std::bind(&roster_parser::parse_unit_optional_mounts, *this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_MOUNTS"] =
            std::bind(&roster_parser::parse_unit_optional_mounts, *this, _1, true, true);
        parsing_functions["OPTIONAL_OCO_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_oco_extras, *this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_OCO_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_oco_extras, *this, _1, true, true);
        parsing_functions["OPTIONAL_MC_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_mc_extras, *this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_MC_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_mc_extras, *this, _1, true, true);
        parsing_functions["OPTIONAL_COMMAND"] =
            std::bind(&roster_parser::parse_unit_optional_command, *this, _1, false, true);
        parsing_functions["BANNER_BUDGET"] =
            std::bind(&roster_parser::parse_unit_banner_budget, *this, _1, false, true);
        parsing_functions["MAGIC_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_mi_budget, *this, _1, false, true);
        parsing_functions["FACTION_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_fi_budget, *this, _1, false, true);
        parsing_functions["TOTAL_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_ti_budget, *this, _1, false, true);
        parsing_functions["UNIQUENESS"] =
            std::bind(&roster_parser::parse_unit_uniqueness, *this, _1, false, true);
    }

    std::vector<std::shared_ptr<base_unit>> roster_parser::parse_() {
        std::vector<std::shared_ptr<base_unit>> roster;
        roster.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            auto search_but = enum_convert::STRING_TO_BASE_UNIT_TYPE.find(read_line(blocks[i] + 1));
            if (search_but == std::end(enum_convert::STRING_TO_BASE_UNIT_TYPE)) {
                std::string msg = "Error parsing " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[i]) + " has an invalid BASE_TYPE argument";
                throw std::runtime_error(msg);
            }
            auto tmp = parse_unit(i, search_but->second);
            roster.push_back(std::move(tmp));
        }
        return roster;
    }

    std::unique_ptr<base_unit> roster_parser::parse_unit(std::size_t block_pos, BaseUnitType but) {
        std::string name = read_line(blocks[block_pos]);
        tpo = tmp_parse_obj();
        std::size_t i = 1U;
        std::string arg = read_line(blocks[block_pos] + i);
        while (!arg.empty()) {
            auto line = tools::split(arg, '=');
            auto search_parse_fn = parsing_functions.find(line[0]);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[block_pos])
                        + " has an invalid argument: " + line[0];
                throw std::runtime_error(msg);
            }
            // call the relevant parsing function
            search_parse_fn->second(tools::trim(line[1]), false, true);
            ++i;
            arg = read_line(blocks[block_pos] + i);
        }
        std::unique_ptr<base_unit> bu;
        switch (but) {
        case BaseUnitType::MAGE_CHARACTER:
            bu = std::make_unique<base_mage_character_unit>(
                faction, tpo.unit_type, tpo.unit_class, name, tpo.points,
                std::move(tpo.characteristics), std::move(tpo.special_rules),
                std::move(tpo.eq), std::move(tpo.opt), tpo.mi_budget,
                tpo.fi_budget, tpo.ti_budget, tpo.unique, tpo.mage_level,
                std::move(tpo.mage_upgrades), std::move(tpo.mage_lores),
                tpo.mount
            );
            break;
        case BaseUnitType::MELEE_CHARACTER:
            bu = std::make_unique<base_melee_character_unit>(
                faction, tpo.unit_type, tpo.unit_class, name, tpo.points,
                std::move(tpo.characteristics), std::move(tpo.special_rules),
                std::move(tpo.eq), std::move(tpo.opt), tpo.mi_budget,
                tpo.fi_budget, tpo.ti_budget, tpo.unique, tpo.mount
            );
            break;
        case BaseUnitType::NORMAL:
            bu = std::make_unique<base_normal_unit>(
                 faction, tpo.unit_type, tpo.unit_class, name, tpo.size.first,
                 tpo.size.second, tpo.points, std::move(tpo.characteristics),
                 std::move(tpo.special_rules), std::move(tpo.eq), std::move(tpo.opt),
                 std::move(tpo.champ_characteristics), std::move(tpo.champ_special_rules),
                 std::move(tpo.champ_eq), std::move(tpo.champ_opt), tpo.mi_budget,
                 tpo.fi_budget, tpo.ti_budget, std::move(tpo.command), tpo.mb_budget,
                 tpo.mount
            );
            break;
        case BaseUnitType::MIXED:
            break;
        default:
            break;
        }
        return bu;
    }

    void roster_parser::parse_unit_type(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.unit_type = enum_convert::STRING_TO_UNIT_TYPE.at(s);
    }
    void roster_parser::parse_unit_category(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.unit_class = enum_convert::STRING_TO_UNIT_CLASS.at(s);
    }
    void roster_parser::parse_unit_mount(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mount = s;
    }
    void roster_parser::parse_unit_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.points = std::stod(s);
    }
    void roster_parser::parse_unit_size(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto min_max = tools::split(s, ',');
        tpo.size.first = static_cast<std::size_t>(std::stoul(min_max[0]));
        tpo.size.second = (min_max[1] != "inf") ?
            static_cast<std::size_t>(std::stoul(min_max[1])) : std::numeric_limits<std::size_t>::max();
    }
    void roster_parser::parse_unit_mage_level(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mage_level = static_cast<short>(std::stoi(s));
    }
    void roster_parser::parse_unit_mage_upgrades(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto upgrades = tools::split(s, ',');
        for (const auto& x : upgrades) {
            auto v = tools::split(x, '[');
            tpo.mage_upgrades[static_cast<short>(std::stoi(v[0]))] = std::stod(tools::split(v[1], ']')[0]);
        }
    }
    void roster_parser::parse_unit_characteristics(const std::string& s, bool champion, bool master) {
        (void)master;
        if (champion) tpo.champ_characteristics = tools::split(s, ' ');
        else tpo.characteristics = tools::split(s, ' ');

    }
    void roster_parser::parse_unit_special_rules(const std::string& s, bool champion, bool master) {
        (void)master;
        if (champion) tpo.champ_special_rules = tools::split(s, ',');
        else tpo.special_rules = tools::split(s, ',');
    }
    void roster_parser::parse_unit_equipment(const std::string& s, bool champion, bool master) {
        (void)master;
        /*std::vector<std::string> all = tools::split(s, ',');
        for (const auto& x : all) {
            std::vector<std::string> v = tools::split(x, ':');
            switch (enum_convert::STRING_TO_ITEM_TYPE.at(v[0])) {
            case ItemType::WEAPON:
            {
                auto item_bs = tools::parse_item_bs(v[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                WeaponType wt = enum_convert::STRING_TO_WEAPON_TYPE.at(item_bs[2]);
                tpo.eq.weapons_[wt] = {ic, name};
                break;
            }
            case ItemType::ARMOUR:
            {
                auto item_bs = tools::parse_item_bs(v[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                ArmourType at = enum_convert::STRING_TO_ARMOUR_TYPE.at(item_bs[2]);
                armour[at] = {ic, name};
                break;
            }
            case ItemType::TALISMAN:
                talismans.push_back(v[1]);
                break;
            case ItemType::ARCANE:
                arcane.push_back(v[1]);
                break;
            case ItemType::ENCHANTED:
                enchanted.push_back(v[1]);
                break;
            case ItemType::BANNER:
                banners.push_back(v[1]);
                break;
            default: break;
            }
        }*/
    }
    void roster_parser::parse_unit_optional_weapons(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_optional_armour(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_optional_mounts(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_optional_oco_extras(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_optional_mc_extras(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_optional_command(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_banner_budget(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_mi_budget(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_fi_budget(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_ti_budget(const std::string& s, bool champion, bool master) {

    }
    void roster_parser::parse_unit_uniqueness(const std::string& s, bool champion, bool master) {

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
        std::vector<std::string> entries = tools::split(s, ',');
        for (const auto& x : entries) {
            std::vector<std::string> tmp = tools::split(x, ':');
            std::vector<std::string> name_pts = tools::split(tmp[1], '[');
            std::string pts_str;
            std::copy_if(
                name_pts[1].begin(), name_pts[1].end(), std::back_inserter(pts_str), [](auto x) { return x != ']'; }
            );
            double pts = std::stod(pts_str);
            map[enum_convert::STRING_TO_COMMAND.at(tmp[0])] = std::make_pair(
                tools::trim(name_pts[0]), pts
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
        std::unordered_map<
            WeaponType,
            std::pair<ItemClass, std::string>
        > weapons;
        std::unordered_map<
            ArmourType,
            std::pair<ItemClass, std::string>
        > armour;
        std::vector<std::string> talismans;
        std::vector<std::string> arcane;
        std::vector<std::string> enchanted;
        std::vector<std::string> banners;
        if (s == "None" || s.empty()) {
            weapons[WeaponType::MELEE] = {ItemClass::MUNDANE, "Hand weapon"};
            return equipment{
                std::move(weapons), std::move(armour), std::move(talismans),
                std::move(arcane), std::move(enchanted), std::move(banners)
            };
        }
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> vec = tools::split(_s, ':');
            switch (enum_convert::STRING_TO_ITEM_TYPE.at(vec[0])) {
            case ItemType::WEAPON:
            {
                auto item_bs = tools::parse_item_bs(vec[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                WeaponType wt = enum_convert::STRING_TO_WEAPON_TYPE.at(item_bs[2]);
                weapons[wt] = {ic, name};
                break;
            }
            case ItemType::ARMOUR:
            {
                auto item_bs = tools::parse_item_bs(vec[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                ArmourType at = enum_convert::STRING_TO_ARMOUR_TYPE.at(item_bs[2]);
                armour[at] = {ic, name};
                break;
            }
            case ItemType::TALISMAN:
                talismans.push_back(vec[1]);
                break;
            case ItemType::ARCANE:
                arcane.push_back(vec[1]);
                break;
            case ItemType::ENCHANTED:
                enchanted.push_back(vec[1]);
                break;
            case ItemType::BANNER:
                banners.push_back(vec[1]);
                break;
            default: break;
            }
        }
        if (!(weapons.count(WeaponType::MELEE)))
            weapons[WeaponType::MELEE] = {ItemClass::MUNDANE, "Hand weapon"};
        return equipment{
            std::move(weapons), std::move(armour), std::move(talismans),
            std::move(arcane), std::move(enchanted), std::move(banners)
        };
    }

    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    > roster_parser::parse_optional_weapons(std::string s) {
        std::unordered_map<
            std::string,
            std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> replace;
            auto split = tools::split(_s, ';');
            if (split.size() > 1) {
                for (auto& r : tools::split(split[1], '/')) replace.push_back(tools::trim(r));
            }
            auto weapon_bsp = tools::parse_item_bsp(split[0]);
            std::string name = tools::trim(weapon_bsp[0]);
            ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(weapon_bsp[1]);
            WeaponType wt = enum_convert::STRING_TO_WEAPON_TYPE.at(weapon_bsp[2]);
            double pts = std::stod(weapon_bsp[3]);
            um[name] = std::make_tuple(wt, ic, pts, replace);
        }
        return um;
    }

    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
    > roster_parser::parse_optional_armour(std::string s) {
        std::unordered_map<
            std::string,
            std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> replace;
            auto split = tools::split(_s, ';');
            if (split.size() > 1) {
                for (auto& r : tools::split(split[1], '/')) replace.push_back(tools::trim(r));
            }
            auto armour_bsp = tools::parse_item_bsp(_s);
            std::string name = tools::trim(armour_bsp[0]);
            ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(armour_bsp[1]);
            ArmourType at = enum_convert::STRING_TO_ARMOUR_TYPE.at(armour_bsp[2]);
            double pts = std::stod(armour_bsp[3]);
            um[name] = std::make_tuple(at, ic, pts, replace);
        }
        return um;
    }

    std::unordered_map<
        std::string,
        double
    > roster_parser::parse_optional_mounts(std::string s) {
        std::unordered_map<
            std::string,
            double
        > um;
        if (s == "None" || s.empty()) return um;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            auto mount_bs = tools::parse_item_bs(_s);
            std::string name = tools::trim(mount_bs[0]);
            //UnitClass uc = enum_convert::STRING_TO_UNIT_CLASS.at(mount_bs[1]);
            double pts = std::stod(mount_bs[2]);
            um[name] = pts;
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
            std::string name = tools::trim(extras_bs[0]);
            bool is_singular = (extras_bs[1] == "True");
            double pts = std::stod(extras_bs[2]);
            um[name] = std::make_pair(is_singular, pts);
        }
        return um;
    }

    std::tuple<double, std::size_t, ItemClass, ItemType> roster_parser::parse_item_budget(const std::string& s) {
        if (s == "None" || s.empty()) return {0.0, 1U, ItemClass::NONE, ItemType::NONE};
        std::vector<std::string> points_w_opts = tools::split(s, ';');
        if (points_w_opts.size() == 1)
            return {std::stod(points_w_opts[0]), 1U, ItemClass::NONE, ItemType::NONE};
        double pts = std::stod(points_w_opts[0]);
        ItemClass ic = ItemClass::NONE;
        std::size_t num = 1U;
        ItemType it = ItemType::NONE;
        std::vector<std::string> opts = tools::split(points_w_opts[1], ',');
        for (const auto& x : opts) {
            std::vector<std::string> y = tools::split(x, ':');
            if (y[0] == "itemclass")
                ic = enum_convert::STRING_TO_ITEM_CLASS.at(y[1]);
            else if (y[0] == "num")
                num = static_cast<std::size_t>(std::stoul(y[1]));
            else if (y[0] == "type")
                it = enum_convert::STRING_TO_ITEM_TYPE.at(y[1]);
        }
        return {pts, num, ic, it};
    }

    std::vector<std::shared_ptr<base_unit>> roster_parser::parse() {
        std::vector<std::shared_ptr<base_unit>> units;
        units.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string types = read_line(blocks[i] + 1);
            auto types_split = tools::split(types, ',');
            UnitType ut = enum_convert::STRING_TO_UNIT_TYPE.at(types_split[0]);
            switch (ut) {
            case UnitType::LORD:
            case UnitType::HERO:
                if (types_split[1] == "Melee")
                    units.push_back(std::make_shared<base_melee_character_unit>(
                        parse_melee_character(i, ut)
                    ));
                else if (types_split[1] == "Mage")
                    units.push_back(std::make_shared<base_mage_character_unit>(
                        parse_mage_character(i, ut)
                    ));
                break;
            case UnitType::CORE:
            case UnitType::SPECIAL:
            case UnitType::RARE:
            {
                auto category = enum_convert::STRING_TO_UNIT_CLASS.at(types_split[1]);
                if (types_split[2] == "Normal") {
                    switch (category) {
                    case UnitClass::MONSTER:
                    case UnitClass::SWARM:
                        units.push_back(
                                  std::make_shared<base_normal_unit>(
                                      parse_minimal_normal_unit(i, ut, category)
                                  )
                              );
                        break;
                    case UnitClass::WARMACHINE:
                    case UnitClass::CHARIOT:
                        if (faction == Faction::TOMB_KINGS &&
                                read_line(blocks[i]) == "Skeleton Chariots")
                            units.push_back(std::make_shared<base_normal_unit>(
                                    parse_normal_unit(i, ut, category)
                                )
                            );
                        else
                            units.push_back(
                                      std::make_shared<base_normal_unit>(
                                          parse_warmachine(i, ut, category)
                                    )
                                );
                        break;
                    case UnitClass::UNIQUE:
                    {
                        auto name = read_line(blocks[i]);
                        if (name == "Gyrocopter" || name == "Gyrobomber")
                            units.push_back(
                                std::make_shared<base_normal_unit>(
                                    parse_gyro_unit(i, ut, category)
                                )
                            );
                        break;
                    }
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

    base_melee_character_unit roster_parser::parse_melee_character(std::size_t n, UnitType ut) {
        std::string name = read_line(blocks[n]);
        auto category = enum_convert::STRING_TO_UNIT_CLASS.at(read_line(blocks[n] + 2));
        double pts = std::stod(read_line(blocks[n] + 3));
        auto stats = tools::split(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 7));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 8));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 9));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 10));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 11));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 12));
        options opt(
            std::move(opt_weapons),
            std::move(opt_armours),
            std::move(opt_mounts),
            std::move(opt_oco_extras),
            std::move(opt_mc_extras)
        );
        double mi_budget = std::stod(read_line(blocks[n] + 13));
        double fi_budget = std::stod(read_line(blocks[n] + 14));
        double ti_budget = std::stod(read_line(blocks[n] + 15));
        bool is_unique = read_line(blocks[n] + 16) == "Unique";
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
            is_unique
        );
        return tmp;
    }

    base_mage_character_unit roster_parser::parse_mage_character(std::size_t n, UnitType ut) {
        std::string name = read_line(blocks[n]);
        short level = static_cast<short>(std::stoi(read_line(blocks[n] + 2)));
        auto level_upgrades = parse_mage_upgrades(read_line(blocks[n] + 3));
        auto lores = tools::split(read_line(blocks[n] + 4), ',');
        auto category = enum_convert::STRING_TO_UNIT_CLASS.at(read_line(blocks[n] + 5));
        double pts = std::stod(read_line(blocks[n] + 6));
        auto stats = tools::split(read_line(blocks[n] + 8), ' ');
        auto rules = tools::split(read_line(blocks[n] + 9), ',');
        auto eq = parse_equipment(read_line(blocks[n] + 10));
        auto opt_weapons = parse_optional_weapons(read_line(blocks[n] + 11));
        auto opt_armours = parse_optional_armour(read_line(blocks[n] + 12));
        auto opt_mounts = parse_optional_mounts(read_line(blocks[n] + 13));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 14));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 15));
        options opt(
            std::move(opt_weapons),
            std::move(opt_armours),
            std::move(opt_mounts),
            std::move(opt_oco_extras),
            std::move(opt_mc_extras)
        );
        double mi_budget = std::stod(read_line(blocks[n] + 16));
        double fi_budget = std::stod(read_line(blocks[n] + 17));
        double ti_budget = std::stod(read_line(blocks[n] + 18));
        bool is_unique = read_line(blocks[n] + 19) == "Unique";
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
            is_unique,
            level,
            std::move(level_upgrades),
            std::move(lores)
        );
        return tmp;
    }

    base_normal_unit roster_parser::parse_normal_unit(
        std::size_t n, UnitType ut, UnitClass category, std::string name_, std::size_t offset
    ) {
        if (name_.empty())
            name_ = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2 + offset));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3 + offset));
        auto stats = tools::split(read_line(blocks[n] + 4 + offset), ' ');
        auto champ_stats = tools::split(read_line(blocks[n] + 5 + offset), ' ');
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
        auto opt_mounts = std::unordered_map<std::string, double>();
        auto champ_opt_mounts = std::unordered_map<std::string, double>();
        options opt(
            std::move(opt_weapons),
            std::move(opt_armours),
            std::move(opt_mounts),
            std::move(opt_oco_extras),
            std::move(opt_mc_extras)
        );
        options champ_opt(
            std::move(champ_opt_weapons),
            std::move(champ_opt_armours),
            std::move(champ_opt_mounts),
            std::move(champ_opt_oco_extras),
            std::move(champ_opt_mc_extras)
        );
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

    base_normal_unit roster_parser::parse_minimal_normal_unit(std::size_t n, UnitType ut, UnitClass category) {
        std::string name = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split(read_line(blocks[n] + 4), ' ');
        std::vector<std::string> champ_stats;
        auto rules = tools::split(read_line(blocks[n] + 5), ',');
        std::vector<std::string> champ_rules;
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 6));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 7));
        equipment eq;
        equipment champ_eq;
        options opt(std::move(opt_oco_extras), std::move(opt_mc_extras));
        options champ_opt;
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

    base_normal_unit roster_parser::parse_warmachine(std::size_t n, UnitType ut, UnitClass category) {
        std::string name = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split(read_line(blocks[n] + 4), ' ');
        auto machine_stats = tools::split(read_line(blocks[n] + 5), ' ');
        auto rules = tools::split(read_line(blocks[n] + 6), ',');
        std::vector<std::string> champ_rules;
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 7));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 8));
        equipment eq;
        equipment champ_eq;
        options opt(std::move(opt_oco_extras), std::move(opt_mc_extras));
        options champ_opt;
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
            std::move(machine_stats),
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

    base_mixed_unit roster_parser::parse_mixed_unit(std::size_t n, UnitType ut, UnitClass category) {
        auto slave = parse_normal_unit(n, ut, category);
        auto master = parse_normal_unit(n, ut, category, slave.name(), 21U);
        base_mixed_unit tmp(
            slave.name(),
            std::move(master),
            std::move(slave)
        );
        return tmp;
    }

    base_normal_unit roster_parser::parse_gyro_unit(
        std::size_t n,
        UnitType ut,
        UnitClass category
    ) {
        std::string name = read_line(blocks[n]);
        double pts = std::stod(read_line(blocks[n] + 2));
        auto mm_size = parse_minmax_size(read_line(blocks[n] + 3));
        auto stats = tools::split(read_line(blocks[n] + 4), ' ');
        std::vector<std::string> champ_stats;
        auto rules = tools::split(read_line(blocks[n] + 5), ',');
        std::vector<std::string> champ_rules;
        auto eq = parse_equipment(read_line(blocks[n] + 6));
        auto opt_oco_extras = parse_optional_extras(read_line(blocks[n] + 7));
        auto opt_mc_extras = parse_optional_extras(read_line(blocks[n] + 8));
        equipment champ_eq;
        options opt(std::move(opt_oco_extras), std::move(opt_mc_extras));
        options champ_opt;
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

}
