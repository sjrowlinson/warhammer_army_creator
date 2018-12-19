#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(const QString& rfile, Faction _faction)
        : file_parser(rfile), faction(_faction), tpo(), curr_block(0U) {
        find_blocks();
        register_bindings();
    }

    void roster_parser::register_bindings() {
        using std::placeholders::_1;
        parsing_functions["TYPE"] =
            std::bind(&roster_parser::parse_unit_type, this, _1, false, true);
        parsing_functions["CATEGORY"] =
            std::bind(&roster_parser::parse_unit_category, this, _1, false, true);
        parsing_functions["MOUNT"] =
            std::bind(&roster_parser::parse_unit_mount, this, _1, false, true);
        parsing_functions["POINTS"] =
            std::bind(&roster_parser::parse_unit_points, this, _1, false, true);
        parsing_functions["SIZE"] =
            std::bind(&roster_parser::parse_unit_size, this, _1, false, true);
        parsing_functions["MAGE_LEVEL"] =
            std::bind(&roster_parser::parse_unit_mage_level, this, _1, false, true);
        parsing_functions["MAGE_UPGRADES"] =
            std::bind(&roster_parser::parse_unit_mage_upgrades, this, _1, false, true);
        parsing_functions["MAGE_LORES"] =
            std::bind(&roster_parser::parse_unit_mage_lores, this, _1, false, true);
        parsing_functions["CHARACTERISTICS"] =
            std::bind(&roster_parser::parse_unit_characteristics, this, _1, false, true);
        parsing_functions["CHAMPION_CHARACTERISTICS"] =
            std::bind(&roster_parser::parse_unit_characteristics, this, _1, true, true);
        parsing_functions["SPECIAL_RULES"] =
            std::bind(&roster_parser::parse_unit_special_rules, this, _1, false, true);
        parsing_functions["CHAMPION_SPECIAL_RULES"] =
            std::bind(&roster_parser::parse_unit_special_rules, this, _1, true, true);
        parsing_functions["EQUIPMENT"] =
            std::bind(&roster_parser::parse_unit_equipment, this, _1, false, true);
        parsing_functions["CHAMPION_EQUIPMENT"] =
            std::bind(&roster_parser::parse_unit_equipment, this, _1, true, true);
        parsing_functions["OPTIONAL_WEAPONS"] =
            std::bind(&roster_parser::parse_unit_optional_weapons, this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_WEAPONS"] =
            std::bind(&roster_parser::parse_unit_optional_weapons, this, _1, true, true);
        parsing_functions["OPTIONAL_ARMOUR"] =
            std::bind(&roster_parser::parse_unit_optional_armour, this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_ARMOUR"] =
            std::bind(&roster_parser::parse_unit_optional_armour, this, _1, true, true);
        parsing_functions["OPTIONAL_MOUNTS"] =
            std::bind(&roster_parser::parse_unit_optional_mounts, this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_MOUNTS"] =
            std::bind(&roster_parser::parse_unit_optional_mounts, this, _1, true, true);
        parsing_functions["OPTIONAL_OCO_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_oco_extras, this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_OCO_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_oco_extras, this, _1, true, true);
        parsing_functions["OPTIONAL_MC_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_mc_extras, this, _1, false, true);
        parsing_functions["CHAMPION_OPTIONAL_MC_EXTRAS"] =
            std::bind(&roster_parser::parse_unit_optional_mc_extras, this, _1, true, true);
        parsing_functions["OPTIONAL_COMMAND"] =
            std::bind(&roster_parser::parse_unit_optional_command, this, _1, false, true);
        parsing_functions["BANNER_BUDGET"] =
            std::bind(&roster_parser::parse_unit_banner_budget, this, _1, false, true);
        parsing_functions["MAGIC_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_mi_budget, this, _1, false, true);
        parsing_functions["FACTION_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_fi_budget, this, _1, false, true);
        parsing_functions["TOTAL_ITEM_BUDGET"] =
            std::bind(&roster_parser::parse_unit_ti_budget, this, _1, false, true);
        parsing_functions["UNIQUENESS"] =
            std::bind(&roster_parser::parse_unit_uniqueness, this, _1, false, true);
    }

    std::vector<std::shared_ptr<base_unit>> roster_parser::parse() {
        std::vector<std::shared_ptr<base_unit>> roster;
        roster.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            auto but_arg = tools::split(read_line(blocks[i] + 1), '=')[1];
            auto search_but = enum_convert::STRING_TO_BASE_UNIT_TYPE.find(but_arg);
            if (search_but == std::end(enum_convert::STRING_TO_BASE_UNIT_TYPE)) {
                std::string msg = "Error parsing " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[i]) + " has an invalid BASE_TYPE argument: "
                        + but_arg;
                throw std::runtime_error(msg);
            }
            try {
                auto tmp = parse_unit(i, search_but->second);
                roster.push_back(std::move(tmp));
            } catch(const std::runtime_error&) { throw; }
        }
        return roster;
    }

    std::unique_ptr<base_unit> roster_parser::parse_unit(std::size_t block_pos, BaseUnitType but) {
        curr_block = block_pos;
        std::string name = read_line(blocks[block_pos]);
        tpo = tmp_parse_obj();
        std::size_t i = 1U;
        std::string arg = read_line(blocks[block_pos] + i);
        while (!arg.empty()) { // iterate over all args in the unit block
            auto line = tools::split(arg, '=');
            if (line[0] == "BASE_TYPE") {
                ++i;
                arg = read_line(blocks[block_pos] + i);
                continue;
            };
            auto search_parse_fn = parsing_functions.find(line[0]);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[block_pos])
                        + " has an invalid argument: " + line[0];
                throw std::runtime_error(msg);
            }
            // call the relevant parsing function
            try { search_parse_fn->second(tools::trim(line[1]), false, true); }
            catch (const std::runtime_error&) { throw; }
            ++i;
            arg = read_line(blocks[block_pos] + i);
        }
        std::unique_ptr<base_unit> bu;
        switch (but) {
        case BaseUnitType::MAGE_CHARACTER:
        {
            equipment eq(std::move(tpo.eq));
            options opt(std::move(tpo.opt));
            bu = std::make_unique<base_mage_character_unit>(
                faction, tpo.unit_type, tpo.unit_class, name, tpo.points,
                std::move(tpo.characteristics), std::move(tpo.special_rules),
                std::move(eq), std::move(opt), tpo.mi_budget,
                tpo.fi_budget, tpo.ti_budget, tpo.unique, tpo.mage_level,
                std::move(tpo.mage_upgrades), std::move(tpo.mage_lores),
                tpo.mount
            );
            break;
        }
        case BaseUnitType::MELEE_CHARACTER:
        {
            equipment eq(std::move(tpo.eq));
            options opt(std::move(tpo.opt));
            bu = std::make_unique<base_melee_character_unit>(
                faction, tpo.unit_type, tpo.unit_class, name, tpo.points,
                std::move(tpo.characteristics), std::move(tpo.special_rules),
                std::move(eq), std::move(opt), tpo.mi_budget,
                tpo.fi_budget, tpo.ti_budget, tpo.unique, tpo.mount
            );
            break;
        }
        case BaseUnitType::NORMAL:
        {
            equipment eq(std::move(tpo.eq));
            options opt(std::move(tpo.opt));
            equipment champ_eq(std::move(tpo.champ_eq));
            options champ_opt(std::move(tpo.champ_opt));
            bu = std::make_unique<base_normal_unit>(
                 faction, tpo.unit_type, tpo.unit_class, name, tpo.size.first,
                 tpo.size.second, tpo.points, std::move(tpo.characteristics),
                 std::move(tpo.special_rules), std::move(eq), std::move(opt),
                 std::move(tpo.champ_characteristics), std::move(tpo.champ_special_rules),
                 std::move(champ_eq), std::move(champ_opt), tpo.mi_budget,
                 tpo.fi_budget, tpo.ti_budget, std::move(tpo.command), tpo.mb_budget,
                 tpo.mount
            );
            break;
        }
        case BaseUnitType::MIXED:
            break;
        default:
            break;
        }
        return bu;
    }

    void roster_parser::parse_unit_type(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto search = enum_convert::STRING_TO_UNIT_TYPE.find(s);
        if (search == std::end(enum_convert::STRING_TO_UNIT_TYPE)) {
            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                    + " roster - unit: " + read_line(blocks[curr_block])
                    + " has an invalid argument TYPE: " + s;
            throw std::runtime_error(msg);
        }
        tpo.unit_type = search->second;
    }
    void roster_parser::parse_unit_category(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto search = enum_convert::STRING_TO_UNIT_CLASS.find(s);
        if (search == std::end(enum_convert::STRING_TO_UNIT_CLASS)) {
            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                    + " roster - unit: " + read_line(blocks[curr_block])
                    + " has an invalid argument CATEGORY: " + s;
            throw std::runtime_error(msg);
        }
        tpo.unit_class = search->second;
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
    void roster_parser::parse_unit_mage_lores(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mage_lores = tools::split(s, ',');
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
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& x : all) {
            std::vector<std::string> v = tools::split(x, ':');
            auto search_item_type = enum_convert::STRING_TO_ITEM_TYPE.find(v[0]);
            if (search_item_type == std::end(enum_convert::STRING_TO_ITEM_TYPE)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument EQUIPMENT: " + s;
                throw std::runtime_error(msg);
            }
            switch (search_item_type->second) {
            case ItemType::WEAPON:
            {
                auto item_bs = tools::parse_item_bs(v[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                WeaponType wt = enum_convert::STRING_TO_WEAPON_TYPE.at(item_bs[2]);
                if (champion) tpo.champ_eq.weapons[wt] = {ic, name};
                else tpo.eq.weapons[wt] = {ic, name};
                break;
            }
            case ItemType::ARMOUR:
            {
                auto item_bs = tools::parse_item_bs(v[1]);
                std::string name = tools::trim(item_bs[0]);
                ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(item_bs[1]);
                ArmourType at = enum_convert::STRING_TO_ARMOUR_TYPE.at(item_bs[2]);
                if (champion) tpo.champ_eq.armour[at] = {ic, name};
                else tpo.eq.armour[at] = {ic, name};
                break;
            }
            case ItemType::TALISMAN:
                if (champion) tpo.champ_eq.talismans.push_back(v[1]);
                else tpo.eq.talismans.push_back(v[1]);
                break;
            case ItemType::ARCANE:
                if (champion) tpo.champ_eq.arcane.push_back(v[1]);
                else tpo.eq.arcane.push_back(v[1]);
                break;
            case ItemType::ENCHANTED:
                if (champion) tpo.champ_eq.enchanted.push_back(v[1]);
                else tpo.eq.enchanted.push_back(v[1]);
                break;
            case ItemType::BANNER:
                if (champion) tpo.champ_eq.banners.push_back(v[1]);
                else tpo.eq.banners.push_back(v[1]);
                break;
            default: break;
            }
        }
    }
    void roster_parser::parse_unit_optional_weapons(const std::string& s, bool champion, bool master) {
        (void)master;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> replace;
            auto split = tools::split(_s, ';');
            if (split.size() > 1) {
                for (auto& r : tools::split(split[1], '/')) replace.push_back(r);
            }
            auto weapon_bsp = tools::parse_item_bsp(split[0]);
            std::string name = tools::trim(weapon_bsp[0]);
            ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(weapon_bsp[1]);
            WeaponType wt = enum_convert::STRING_TO_WEAPON_TYPE.at(weapon_bsp[2]);
            double pts = std::stod(weapon_bsp[3]);
            if (champion) tpo.champ_opt.weapons[name] = std::make_tuple(wt, ic, pts, replace);
            else tpo.opt.weapons[name] = std::make_tuple(wt, ic, pts, replace);
        }
    }
    void roster_parser::parse_unit_optional_armour(const std::string& s, bool champion, bool master) {
        (void)master;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& _s : all) {
            std::vector<std::string> replace;
            auto split = tools::split(_s, ';');
            if (split.size() > 1) {
                for (auto& r : tools::split(split[1], '/')) replace.push_back(r);
            }
            auto armour_bsp = tools::parse_item_bsp(split[0]);
            std::string name = tools::trim(armour_bsp[0]);
            ItemClass ic = enum_convert::STRING_TO_ITEM_CLASS.at(armour_bsp[1]);
            ArmourType at = enum_convert::STRING_TO_ARMOUR_TYPE.at(armour_bsp[2]);
            double pts = std::stod(armour_bsp[3]);
            if (champion) tpo.champ_opt.armour[name] = std::make_tuple(at, ic, pts, replace);
            else tpo.opt.armour[name] = std::make_tuple(at, ic, pts, replace);
        }
    }
    void roster_parser::parse_unit_optional_mounts(const std::string& s, bool champion, bool master) {
        (void)master;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& x : all) {
            auto name_pts = tools::split(x, '[');
            if (champion) tpo.champ_opt.mounts[name_pts[0]] = std::stod(tools::split(name_pts[1], ']')[0]);
            else tpo.opt.mounts[name_pts[0]] = std::stod(tools::split(name_pts[1], ']')[0]);
        }
    }
    void roster_parser::parse_unit_optional_oco_extras(const std::string& s, bool champion, bool master) {
        (void)master;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& x : all) {
            auto extras_bs = tools::parse_item_bs(x);
            std::string name = tools::trim(extras_bs[0]);
            bool is_singular = (extras_bs[1] == "True");
            double pts = std::stod(extras_bs[2]);
            if (champion) tpo.champ_opt.oco_extras[name] = std::make_pair(is_singular, pts);
            else tpo.opt.oco_extras[name] = std::make_pair(is_singular, pts);
        }
    }
    void roster_parser::parse_unit_optional_mc_extras(const std::string& s, bool champion, bool master) {
        (void)master;
        std::vector<std::string> all = tools::split(s, ',');
        for (const auto& x : all) {
            auto extras_bs = tools::parse_item_bs(x);
            std::string name = tools::trim(extras_bs[0]);
            bool is_singular = (extras_bs[1] == "True");
            double pts = std::stod(extras_bs[2]);
            if (champion) tpo.champ_opt.mc_extras[name] = std::make_pair(is_singular, pts);
            else tpo.opt.mc_extras[name] = std::make_pair(is_singular, pts);
        }
    }
    void roster_parser::parse_unit_optional_command(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto members = tools::split(s, ',');
        for (const auto& m : members) {
            auto type_name = tools::split(m, ':');
            auto name_pts = tools::split(type_name[1], '[');
            tpo.command[enum_convert::STRING_TO_COMMAND.at(type_name[0])]
                = std::make_pair(name_pts[0], std::stod(tools::split(name_pts[1], ']')[0]));
        }
    }
    void roster_parser::parse_unit_banner_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mb_budget = std::stod(s);
    }
    void roster_parser::parse_unit_mi_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mi_budget = std::stod(s);
    }
    void roster_parser::parse_unit_fi_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.fi_budget = std::stod(s);
    }
    void roster_parser::parse_unit_ti_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.ti_budget = std::stod(s);
    }
    void roster_parser::parse_unit_uniqueness(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.unique = s == "true";
    }

    /*std::tuple<double, std::size_t, ItemClass, ItemType> roster_parser::parse_item_budget(const std::string& s) {
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
    }*/
}
