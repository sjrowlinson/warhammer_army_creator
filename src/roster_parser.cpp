#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(const QString& rfile, Faction _faction)
        : file_parser(rfile),
          faction(_faction), tpo(), curr_block(0U), curr_line(0U), in_multiline_state(false) {
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
            auto but_args = tools::split(read_line(blocks[i] + 1), '=');
            if (but_args[0] != "BASE_TYPE") {
                std::string msg = "Error parsing " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[i])
                        + " does not have BASE_TYPE argument as first line.";
                throw std::runtime_error(msg);
            }
            auto search_but = enum_convert::STRING_TO_BASE_UNIT_TYPE.find(but_args[1]);
            if (search_but == std::end(enum_convert::STRING_TO_BASE_UNIT_TYPE)) {
                std::string msg = "Error parsing " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[i]) + " has an invalid BASE_TYPE argument: "
                        + but_args[1];
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
        std::size_t i = 2U;
        std::string arg = read_line(blocks[block_pos] + i);
        while (!arg.empty()) { // iterate over all args in the unit block
            curr_line = i;
            auto line = tools::split(arg, '=');
            auto search_parse_fn = parsing_functions.find(line[0]);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[block_pos])
                        + " has an invalid argument: " + line[0];
                throw std::runtime_error(msg);
            }
            in_multiline_state = tools::starts_with(line[1], '[');
            // call the relevant parsing function
            std::size_t ml_offset = 0U;
            try { ml_offset = search_parse_fn->second(tools::trim(line[1]), false, true); }
            catch (const std::runtime_error&) { throw; }
            i += 1U + ml_offset;
            arg = read_line(blocks[block_pos] + i);
        }
        if (!(tpo.unit_category == UnitCategory::CHARIOT || tpo.unit_category == UnitCategory::MONSTER
              || tpo.unit_category == UnitCategory::UNIQUE || tpo.unit_category == UnitCategory::WARBEASTS
              || tpo.unit_category == UnitCategory::WARMACHINE)) {
            if (!tpo.eq.weapons.count(WeaponType::MELEE))
                tpo.eq.weapons[WeaponType::MELEE] = {ItemCategory::MUNDANE, "Hand weapon"};
            switch (tpo.unit_type) {
            case UnitType::CORE:
            case UnitType::SPECIAL:
            case UnitType::RARE:
                if (!tpo.champ_eq.weapons.count(WeaponType::MELEE))
                    tpo.champ_eq.weapons[WeaponType::MELEE] = {ItemCategory::MUNDANE, "Hand weapon"};
                break;
            default: break;
            }
        }
        std::unique_ptr<base_unit> bu;
        switch (but) {
        case BaseUnitType::MAGE_CHARACTER:
        {
            equipment eq(std::move(tpo.eq));
            options opt(std::move(tpo.opt));
            bu = std::make_unique<base_mage_character_unit>(
                faction, tpo.unit_type, tpo.unit_category, name, tpo.points,
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
                faction, tpo.unit_type, tpo.unit_category, name, tpo.points,
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
                 faction, tpo.unit_type, tpo.unit_category, name, tpo.size.first,
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

    std::size_t roster_parser::parse_unit_type(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto search = enum_convert::STRING_TO_UNIT_TYPE.find(s);
        if (search == std::end(enum_convert::STRING_TO_UNIT_TYPE)) {
            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                    + " roster - unit: " + read_line(blocks[curr_block])
                    + " has an invalid TYPE argument: " + s;
            throw std::runtime_error(msg);
        }
        tpo.unit_type = search->second;
        return 0U;
    }
    std::size_t roster_parser::parse_unit_category(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto search = enum_convert::STRING_TO_UNIT_CLASS.find(s);
        if (search == std::end(enum_convert::STRING_TO_UNIT_CLASS)) {
            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                    + " roster - unit: " + read_line(blocks[curr_block])
                    + " has an invalid CATEGORY argument: " + s;
            throw std::runtime_error(msg);
        }
        tpo.unit_category = search->second;
        return 0U;
    }
    std::size_t roster_parser::parse_unit_mount(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mount = s;
        return 0U;
    }
    std::size_t roster_parser::parse_unit_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.points = std::stod(s);
        return 0U;
    }
    std::size_t roster_parser::parse_unit_size(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto min_max = tools::split(s, ',');
        tpo.size.first = static_cast<std::size_t>(std::stoul(min_max[0]));
        tpo.size.second = (min_max[1] != "inf") ?
            static_cast<std::size_t>(std::stoul(min_max[1])) : std::numeric_limits<std::size_t>::max();
        return 0U;
    }
    std::size_t roster_parser::parse_unit_mage_level(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mage_level = static_cast<short>(std::stoi(s));
        return 0U;
    }
    std::size_t roster_parser::parse_unit_mage_upgrades(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto upgrades = tools::split(s, ',');
        for (const auto& x : upgrades) {
            auto v = tools::split(x, '[');
            tpo.mage_upgrades[static_cast<short>(std::stoi(v[0]))] = std::stod(tools::split(v[1], ']')[0]);
        }
        return 0U;
    }
    std::size_t roster_parser::parse_unit_mage_lores(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mage_lores = tools::split(s, ',');
        return 0U;
    }
    std::size_t roster_parser::parse_unit_characteristics(const std::string& s, bool champion, bool master) {
        (void)master;
        if (champion) tpo.champ_characteristics = tools::split(s, ' ');
        else tpo.characteristics = tools::split(s, ' ');
        return 0U;
    }
    std::size_t roster_parser::parse_unit_special_rules(const std::string& s, bool champion, bool master) {
        (void)master;
        if (champion) tpo.champ_special_rules = tools::split(s, ',');
        else tpo.special_rules = tools::split(s, ',');
        return 0U;
    }
    std::size_t roster_parser::parse_unit_equipment(const std::string& s, bool champion, bool master) {
        (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            std::vector<std::string> v = tools::split(x, '-');
            auto search_item_type = enum_convert::STRING_TO_ITEM_TYPE.find(v[0]);
            if (search_item_type == std::end(enum_convert::STRING_TO_ITEM_TYPE)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument EQUIPMENT: " + s;
                throw std::runtime_error(msg);
            }
            auto args = tools::split(v[1], ',');
            auto names_values = tools::zip_args_to_names_values(args);
            std::string name;
            ItemCategory ic = ItemCategory::NONE;
            WeaponType wt = WeaponType::NONE;
            ArmourType at = ArmourType::NONE;
            for (const auto& y : names_values) {
                if (y.first == "Name") name = y.second;
                else if (y.first == "Category") {
                    auto search_category = enum_convert::STRING_TO_ITEM_CLASS.find(y.second);
                    if (search_category == std::end(enum_convert::STRING_TO_ITEM_CLASS)) {
                        std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                + " roster - unit: " + read_line(blocks[curr_block])
                                + " has an invalid argument EQUIPMENT with CATEGORY: " + y.second;
                        throw std::runtime_error(msg);
                    }
                    ic = search_category->second;
                }
                else if (y.first == "Type") {
                    switch (search_item_type->second) {
                    case ItemType::WEAPON:
                    {
                        auto search_weapon_type = enum_convert::STRING_TO_WEAPON_TYPE.find(y.second);
                        if (search_weapon_type == std::end(enum_convert::STRING_TO_WEAPON_TYPE)) {
                            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                    + " roster - unit: " + read_line(blocks[curr_block])
                                    + " has an invalid argument EQUIPMENT with Weapon - Type: " + y.second;
                            throw std::runtime_error(msg);
                        }
                        wt = search_weapon_type->second;
                        break;
                    }
                    case ItemType::ARMOUR:
                    {
                        auto search_armour_type = enum_convert::STRING_TO_ARMOUR_TYPE.find(y.second);
                        if (search_armour_type == std::end(enum_convert::STRING_TO_ARMOUR_TYPE)) {
                            std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                    + " roster - unit: " + read_line(blocks[curr_block])
                                    + " has an invalid argument EQUIPMENT with Armour - Type: " + y.second;
                            throw std::runtime_error(msg);
                        }
                        at = search_armour_type->second;
                        break;
                    }
                    default: break;
                    }
                }
            }
            switch (search_item_type->second) {
            case ItemType::WEAPON:
                if (champion) tpo.champ_eq.weapons[wt] = {ic, name};
                else tpo.eq.weapons[wt] = {ic, name};
                break;
            case ItemType::ARMOUR:
                if (champion) tpo.champ_eq.armour[at] = {ic, name};
                else tpo.eq.armour[at] = {ic, name};
                break;
            case ItemType::TALISMAN:
                if (champion) tpo.champ_eq.talismans.push_back(name);
                else tpo.eq.talismans.push_back(name);
                break;
            case ItemType::ARCANE:
                if (champion) tpo.champ_eq.arcane.push_back(name);
                else tpo.eq.arcane.push_back(name);
                break;
            case ItemType::ENCHANTED:
                if (champion) tpo.champ_eq.enchanted.push_back(name);
                else tpo.eq.enchanted.push_back(name);
                break;
            case ItemType::BANNER:
                if (champion) tpo.champ_eq.banners.push_back(name);
                else tpo.eq.banners.push_back(name);
                break;
            default: break;
            }
        }
        // add hand weapon as default if no other melee weapon in equipment
        if (champion) {
            if (!tpo.champ_eq.weapons.count(WeaponType::MELEE))
                tpo.champ_eq.weapons[WeaponType::MELEE] = {ItemCategory::MUNDANE, "Hand weapon"};
        }
        else {
            if (!tpo.eq.weapons.count(WeaponType::MELEE))
                tpo.eq.weapons[WeaponType::MELEE] = {ItemCategory::MUNDANE, "Hand weapon"};
        }
        return ml_pair.second;
    }
    std::size_t roster_parser::parse_unit_optional_weapons(const std::string& s, bool champion, bool master) {
        (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            auto args = tools::split(x, ',');
            auto names_values = tools::zip_args_to_names_values(args);
            if (!tools::in_names_values(names_values, "Name")) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument OPTIONAL_WEAPONS where an item"
                        + " has been specified without a Name argument.";
                throw std::runtime_error(msg);
            }
            weapon_option wo;
            for (const auto& y : names_values) {
                if (y.first == "Name") wo.name = y.second;
                else if (y.first == "Category") {
                    auto search_category = enum_convert::STRING_TO_ITEM_CLASS.find(y.second);
                    if (search_category == std::end(enum_convert::STRING_TO_ITEM_CLASS)) {
                        std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                + " roster - unit: " + read_line(blocks[curr_block])
                                + " has an invalid argument OPTIONAL_WEAPONS with Category: " + y.second;
                        throw std::runtime_error(msg);
                    }
                    wo.category = search_category->second;
                }
                else if (y.first == "Type") {
                    auto search_weapon_type = enum_convert::STRING_TO_WEAPON_TYPE.find(y.second);
                    if (search_weapon_type == std::end(enum_convert::STRING_TO_WEAPON_TYPE)) {
                        std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                + " roster - unit: " + read_line(blocks[curr_block])
                                + " has an invalid argument OPTIONAL_WEAPONS with Type: " + y.second;
                        throw std::runtime_error(msg);
                    }
                    wo.type = search_weapon_type->second;
                }
                else if (y.first == "Points") wo.points = std::stod(y.second);
                else if (y.first == "Replacements") {
                    auto repl = tools::split(y.second, '/');
                    for (auto&& r : repl) wo.replacements.push_back(std::move(r));
                }
                else if (y.first == "Restrictions")
                    wo.restrictions = parse_restrictions(y.second, "OPTIONAL_WEAPONS");
            }
            if (champion) tpo.champ_opt.weapons[wo.name] = wo;
            else tpo.opt.weapons[wo.name] = wo;
        }
        return ml_pair.second;
    }
    std::size_t roster_parser::parse_unit_optional_armour(const std::string& s, bool champion, bool master) {
        (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            auto args = tools::split(x, ',');
            auto names_values = tools::zip_args_to_names_values(args);
            if (!tools::in_names_values(names_values, "Name")) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument OPTIONAL_ARMOUR where an item"
                        + " has been specified without a Name argument.";
                throw std::runtime_error(msg);
            }
            armour_option ao;
            for (const auto& y : names_values) {
                if (y.first == "Name") ao.name = y.second;
                else if (y.first == "Category") {
                    auto search_category = enum_convert::STRING_TO_ITEM_CLASS.find(y.second);
                    if (search_category == std::end(enum_convert::STRING_TO_ITEM_CLASS)) {
                        std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                + " roster - unit: " + read_line(blocks[curr_block])
                                + " has an invalid argument OPTIONAL_ARMOUR with Category: " + y.second;
                        throw std::runtime_error(msg);
                    }
                    ao.category = search_category->second;
                }
                else if (y.first == "Type") {
                    auto search_armour_type = enum_convert::STRING_TO_ARMOUR_TYPE.find(y.second);
                    if (search_armour_type == std::end(enum_convert::STRING_TO_ARMOUR_TYPE)) {
                        std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                                + " roster - unit: " + read_line(blocks[curr_block])
                                + " has an invalid argument OPTIONAL_ARMOUR with Armour - Type: " + y.second;
                        throw std::runtime_error(msg);
                    }
                    ao.type = search_armour_type->second;
                }
                else if (y.first == "Points") ao.points = std::stod(y.second);
                else if (y.first == "Replacements") {
                    auto repl = tools::split(y.second, '/');
                    for (auto&& r : repl) ao.replacements.push_back(std::move(r));
                }
                else if (y.first == "Restrictions")
                    ao.restrictions = parse_restrictions(y.second, "OPTIONAL_ARMOUR");
            }
            if (champion) tpo.champ_opt.armour[ao.name] = ao;
            else tpo.opt.armour[ao.name] = ao;
        }
        return ml_pair.second;
    }
    std::size_t roster_parser::parse_unit_optional_mounts(const std::string& s, bool champion, bool master) {
        (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            auto args = tools::split(x, ',');
            auto names_values = tools::zip_args_to_names_values(args);
            if (!tools::in_names_values(names_values, "Name")) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument OPTIONAL_WEAPONS where an item"
                        + " has been specified without a Name argument.";
                throw std::runtime_error(msg);
            }
            std::string name;
            double points = 0.0;
            for (const auto& y : names_values) {
                if (y.first == "Name") name = y.second;
                else if (y.first == "Points") points = std::stod(y.second);
                else if (y.first == "Replacements") {
                    // TODO: parse replacements
                }
                else if (y.first == "Restrictions") {
                    // TODO: parse restrictions
                }
            }
            if (champion) tpo.champ_opt.mounts[name] = points;
            else tpo.opt.mounts[name] = points;
        }
        return ml_pair.second;
    }

    std::size_t roster_parser::parse_unit_extras(const std::string& s, bool champion, bool master, bool oco) {
        (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            auto args = tools::split(x, ',');
            auto names_values = tools::zip_args_to_names_values(args);
            if (!tools::in_names_values(names_values, "Name")) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument " + std::string(oco ? "OPTIONAL_OCO_EXTRAS" : "OPTIONAL_MC_EXTRAS")
                        + " OPTIONAL_OCO_EXTRAS where an item"
                        + " has been specified without a Name argument.";
                throw std::runtime_error(msg);
            }
            extra_option eo; eo.is_singular = false;
            for (const auto& y : names_values) {
                if (y.first == "Name") eo.name = y.second;
                else if (y.first == "Points") eo.points = std::stod(y.second);
                else if (y.first == "Singular") eo.is_singular = y.second == "true";
                else if (y.second == "Replacements") {
                    auto repl = tools::split(y.second, '/');
                    for (auto&& r : repl) eo.replacements.push_back(std::move(r));
                }
                else if (y.first == "Restrictions") {
                    eo.restrictions = parse_restrictions(y.second, oco ? "OPTIONAL_OCO_EXTRAS" : "OPTIONAL_MC_EXTRAS");
                }
            }
            if (oco) {
                if (champion) tpo.champ_opt.oco_extras[eo.name] = eo;
                else tpo.opt.oco_extras[eo.name] = eo;
            } else {
                if (champion) tpo.champ_opt.mc_extras[eo.name] = eo;
                else tpo.opt.mc_extras[eo.name] = eo;
            }
        }
        return ml_pair.second;
    }

    std::size_t roster_parser::parse_unit_optional_oco_extras(const std::string& s, bool champion, bool master) {
        return parse_unit_extras(s, champion, master, true);
    }
    std::size_t roster_parser::parse_unit_optional_mc_extras(const std::string& s, bool champion, bool master) {
        return parse_unit_extras(s, champion, master, false);
    }
    std::size_t roster_parser::parse_unit_optional_command(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        std::pair<std::string, std::size_t> ml_pair = {"", 0U};
        if (in_multiline_state) ml_pair = multiline_state_handler(s);
        std::vector<std::string> all = tools::split(
            in_multiline_state ? ml_pair.first : s, ';'
        );
        for (const auto& x : all) {
            auto v = tools::split(x, '-');
            auto search_command = enum_convert::STRING_TO_COMMAND.find(v[0]);
            if (search_command == std::end(enum_convert::STRING_TO_COMMAND)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument OPTIONAL_COMMAND with " + v[0];
                throw std::runtime_error(msg);
            }
            auto args = tools::split(v[1], ',');
            auto names_values = tools::zip_args_to_names_values(args);
            std::string name;
            double points = 0.0;
            for (const auto& y : names_values) {
                if (y.first == "Name") name = y.second;
                else if (y.first == "Points") points = std::stod(y.second);
                else if (y.first == "Replacements") {
                    // TODO: parse replacements
                }
                else if (y.first == "Restrictions") {
                    // TODO: parse restrictions
                }
            }
            tpo.command[search_command->second] = std::make_pair(name, points);
        }
        return ml_pair.second;
    }
    std::size_t roster_parser::parse_unit_banner_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mb_budget = std::stod(s);
        return 0U;
    }
    std::size_t roster_parser::parse_unit_mi_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.mi_budget = std::stod(s);
        return 0U;
    }
    std::size_t roster_parser::parse_unit_fi_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.fi_budget = std::stod(s);
        return 0U;
    }
    std::size_t roster_parser::parse_unit_ti_budget(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.ti_budget = std::stod(s);
        return 0U;
    }
    std::size_t roster_parser::parse_unit_uniqueness(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        tpo.unique = s == "true";
        return 0U;
    }

    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > roster_parser::parse_restrictions(const std::string& s, std::string from) {
        std::unordered_map<
            RestrictionField,
            std::vector<std::string>
        > restrictions;
        auto restr = tools::split(s, '/');
        for (const auto& r : restr) {
            auto field_value = tools::split(r, '-');
            auto search_restr_field = enum_convert::STRING_TO_RESTRICTION.find(field_value[0]);
            if (search_restr_field == std::end(enum_convert::STRING_TO_RESTRICTION)) {
                std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument " + from + " with Restrictions: " + field_value[0];
                throw std::runtime_error(msg);
            }
            restrictions[search_restr_field->second].push_back(field_value[1]);
        }
        return restrictions;
    }

    std::pair<std::string, std::size_t> roster_parser::multiline_state_handler(const std::string& s) {
        auto _s = s.substr(1U); // remove '[' at start of block
        std::size_t j = 0U;
        std::string arg = _s;
        while (!tools::ends_with(arg, ']')) {
            arg = read_line(blocks[curr_block] + curr_line + ++j);
            _s += ';' + arg;
        }
        _s.erase(--std::end(_s)); // remove ']' at end of block
        return {_s, j};
    }

    /*std::tuple<double, std::size_t, ItemCategory, ItemType> roster_parser::parse_item_budget(const std::string& s) {
        if (s == "None" || s.empty()) return {0.0, 1U, ItemCategory::NONE, ItemType::NONE};
        std::vector<std::string> points_w_opts = tools::split(s, ';');
        if (points_w_opts.size() == 1)
            return {std::stod(points_w_opts[0]), 1U, ItemCategory::NONE, ItemType::NONE};
        double pts = std::stod(points_w_opts[0]);
        ItemCategory ic = ItemCategory::NONE;
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
