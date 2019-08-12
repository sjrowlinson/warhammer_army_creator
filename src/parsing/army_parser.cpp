#include "army_parser.h"

namespace tools {

    army_parser::army_parser(const QString& file, std::shared_ptr<selection_tree> st)
        : file_parser(file),
        selector(st), curr_line(0U), general_id_(-1) {
        find_blocks();
        register_bindings();
    }

    const std::set<int>& army_parser::all_ids() const noexcept {
        return ids;
    }

    int army_parser::general_id() const noexcept {
        return general_id_;
    }

    void army_parser::register_bindings() {
        using std::placeholders::_1;
        parsing_functions["ID"] =
            std::bind(&army_parser::parse_unit_id, this, _1, false, true);
        parsing_functions["POINTS"] =
            std::bind(&army_parser::parse_unit_points, this, _1, false, true);
        parsing_functions["MAGIC_ITEM_POINTS"] =
            std::bind(&army_parser::parse_unit_magic_item_points, this, _1, false, true);
        parsing_functions["FACTION_ITEM_POINTS"] =
            std::bind(&army_parser::parse_unit_faction_item_points, this, _1, false, true);
        parsing_functions["TOTAL_ITEM_POINTS"] =
            std::bind(&army_parser::parse_unit_total_item_points, this, _1, false, true);
        parsing_functions["ASSIGNED_NAME"] =
            std::bind(&army_parser::parse_unit_assigned_name, this, _1, false, true);
        // melee weapons
        parsing_functions["MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, false, true);
        parsing_functions["CHAMPION_MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, true, true);
        parsing_functions["MASTER_MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, false, true);
        parsing_functions["SLAVE_MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_MELEE_WEAPON"] =
            std::bind(&army_parser::parse_unit_melee_weapon, this, _1, true, false);
        // ranged weapons
        parsing_functions["BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, false, true);
        parsing_functions["CHAMPION_BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, true, true);
        parsing_functions["MASTER_BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, false, true);
        parsing_functions["SLAVE_BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_BALLISTIC_WEAPON"] =
            std::bind(&army_parser::parse_unit_ranged_weapon, this, _1, true, false);
        // body armour
        parsing_functions["ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, false, true);
        parsing_functions["CHAMPION_ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, true, true);
        parsing_functions["MASTER_ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, false, true);
        parsing_functions["SLAVE_ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_ARMOUR_ARMOUR"] =
            std::bind(&army_parser::parse_unit_body_armour, this, _1, true, false);
        // shield
        parsing_functions["SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, false, true);
        parsing_functions["CHAMPION_SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, true, true);
        parsing_functions["MASTER_SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, false, true);
        parsing_functions["SLAVE_SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_SHIELD_ARMOUR"] =
            std::bind(&army_parser::parse_unit_shield_armour, this, _1, true, false);
        // helmet
        parsing_functions["HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, false, true);
        parsing_functions["CHAMPION_HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, true, true);
        parsing_functions["MASTER_HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, false, true);
        parsing_functions["SLAVE_HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_HELMET_ARMOUR"] =
            std::bind(&army_parser::parse_unit_helmet_armour, this, _1, true, false);
        // oco extra
        parsing_functions["OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, false, true);
        parsing_functions["CHAMPION_OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, true, true);
        parsing_functions["MASTER_OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, false, true);
        parsing_functions["SLAVE_OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_OCO_EXTRA"] =
            std::bind(&army_parser::parse_unit_oco_extra, this, _1, true, false);
        // mc extras
        parsing_functions["MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, false, true);
        parsing_functions["CHAMPION_MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, true, true);
        parsing_functions["MASTER_MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, false, true);
        parsing_functions["SLAVE_MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, false, false);
        parsing_functions["MASTER_CHAMPION_MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, true, true);
        parsing_functions["SLAVE_CHAMPION_MC_EXTRAS"] =
            std::bind(&army_parser::parse_unit_mc_extras, this, _1, true, false);
        // talisman
        parsing_functions["TALISMAN"] =
            std::bind(&army_parser::parse_unit_talisman, this, _1, false, true);
        // enchanted item
        parsing_functions["ENCHANTED_ITEM"] =
            std::bind(&army_parser::parse_unit_enchanted_item, this, _1, false, true);
        // arcane item
        parsing_functions["ARCANE_ITEM"] =
            std::bind(&army_parser::parse_unit_arcane_item, this, _1, false, true);
        // item extras
        parsing_functions["MAGIC_ITEM_EXTRAS"] =
            std::bind(&army_parser::parse_unit_item_extras, this, _1, false, true);
        // banner
        parsing_functions["BANNER"] =
            std::bind(&army_parser::parse_unit_banner, this, _1, false, true);
        // size
        parsing_functions["SIZE"] =
            std::bind(&army_parser::parse_unit_size, this, _1, false, true);
        parsing_functions["MASTER_SIZE"] =
            std::bind(&army_parser::parse_unit_size, this, _1, true, true);
        parsing_functions["SLAVE_SIZE"] =
            std::bind(&army_parser::parse_unit_size, this, _1, true, false);
        // command
        parsing_functions["COMMAND"] =
            std::bind(&army_parser::parse_unit_command, this, _1, false, true);
        parsing_functions["MASTER_COMMAND"] =
            std::bind(&army_parser::parse_unit_command, this, _1, true, true);
        parsing_functions["SLAVE_COMMAND"] =
            std::bind(&army_parser::parse_unit_command, this, _1, true, false);
        // mounts
        parsing_functions["MOUNT"] =
            std::bind(&army_parser::parse_unit_mount, this, _1, false, true);
        parsing_functions["MOUNT_OCO_OPTION"] =
            std::bind(&army_parser::parse_unit_mount_oco_option, this, _1, false, true);
        parsing_functions["MOUNT_MC_OPTIONS"] =
            std::bind(&army_parser::parse_unit_mount_mc_options, this, _1, false, true);
        // mage stuff
        parsing_functions["MAGE_LEVEL"] =
            std::bind(&army_parser::parse_unit_mage_level, this, _1, false, true);
        parsing_functions["MAGE_LORES"] =
            std::bind(&army_parser::parse_unit_mage_lores, this, _1, false, true);
    }

    Faction army_parser::parse_faction() {
        auto it = std::find_if(std::begin(blocks), std::end(blocks), [this](std::size_t block_pos) {
            return tools::starts_with(this->read_line(block_pos), "ARMY = ");
        });
        if (it == std::end(blocks)) {
            std::string msg = "Army file: " + filename + " is corrupted. Could not find an ARMY field in the file.";
            throw std::runtime_error(msg);
        }
        std::string race_str = tools::split(read_line(*it), '=')[1];
        if (!enum_convert::STRING_TO_FACTION.count(race_str)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unrecognised value: "
                                + race_str + " in ARMY field.";
            throw std::runtime_error(msg);
        }
        return enum_convert::STRING_TO_FACTION.at(race_str);
    }

    double army_parser::parse() {
        double pts_lim = 0.0;
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            auto line = tools::split(read_line(blocks[i]), '=');
            if (line[0] == "POINTS_LIMIT") {
                pts_lim = std::stod(line[1]);
            }
            else if (line[0] == "GENERAL") {
                general_id_ = std::stoi(line[1]);
            }
            else if (line[0] == "UNIT") {
                std::string unit_name = line[1];
                selector->change_selection(unit_name);
                unit_handle = selector->selected();
                if (unit_handle == nullptr) {
                    std::string msg = "Army file: " + filename + " is corrupted. Unable to convert UNIT: " + unit_name
                                        + " to the correct unit type.";
                    throw std::runtime_error(msg);
                }
                try {
                    parse_unit(i);
                    selector->add_unit_to_army_list(unit_handle->id());
                } catch (const std::runtime_error&) { throw; }
            }
        }
        return pts_lim;
    }

    void army_parser::parse_unit(std::size_t block) {
        curr_block = block;
        std::size_t first_pass_counter = 1;
        auto pre_arg = read_line(blocks[block] + first_pass_counter);
        while (!pre_arg.empty()) {
            auto line = tools::split(pre_arg, '=');
            auto kwd = line[0];
            if (line.size() != 2U) {
                std::string msg = "Army file: " + filename + " is corrupted. Unexpected number of arguments "
                                    "given for keyword: " + kwd + " in unit: " + unit_handle->name();
                throw std::runtime_error(msg);
            }
            auto value = tools::trim(line[1]);
            if (kwd == "COMMAND" || kwd == "SIZE") {
                parsing_functions[kwd](value, false, true);
            } else if (kwd == "MASTER_COMMAND" || kwd == "MASTER_SIZE") {
                parsing_functions[kwd](value, true, true);
            } else if (kwd == "SLAVE_COMMAND" || kwd == "SLAVE_SIZE") {
                parsing_functions[kwd](value, true, false);
            }
            ++first_pass_counter;
            pre_arg = read_line(blocks[block] + first_pass_counter);
        }
        std::size_t i = 1U;
        std::string arg = read_line(blocks[block] + i);
        // TODO  && (block + i) != blocks[block + 1] but handle when blocks[block] is last element
        while (!arg.empty()) {
            curr_line = i;
            auto line = tools::split(arg, '=');
            auto kwd = line[0];
            if (kwd == "COMMAND" || kwd == "MASTER_COMMAND" || kwd == "SLAVE_COMMAND" ||
                    kwd == "SIZE" || kwd == "MASTER_SIZE" || kwd == "SLAVE_SIZE") {
                ++i;
                arg = read_line(blocks[block] + i);
                continue;
            }
            auto value = line[1];
            auto search_parse_fn = parsing_functions.find(kwd);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Army file: " + filename + " is corrupted. Unexpected keyword: " + kwd
                                    + " in unit: " + unit_handle->name();
                throw std::runtime_error(msg);
            }
            try {
                search_parse_fn->second(tools::trim(value), false, true);
            } catch (const std::runtime_error&) { throw; }
            ++i;
            arg = read_line(blocks[block] + i);
        }
        if (!cached_mixed_unit_master_size.empty()) {
            parse_unit_size(cached_mixed_unit_master_size, true, true);
            cached_mixed_unit_master_size.clear();
        }
        if (!tools::isclose(unit_handle->magic_item_points(), unit_magic_pts)) {
            std::string msg = "Army file: " + filename + " is corrupted. Mismatch in magic item points values"
                                " in unit: " + unit_handle->name() + " of " +
                                tools::points_str(unit_handle->magic_item_points() - unit_magic_pts) + " pts";
            throw std::runtime_error(msg);
        }
        if (!tools::isclose(unit_handle->faction_item_points(), unit_faction_pts)) {
            std::string msg = "Army file: " + filename + " is corrupted. Mismatch in faction item points values"
                                " in unit: " + unit_handle->name() + " of " +
                                tools::points_str(unit_handle->faction_item_points() - unit_faction_pts) + " pts";
            throw std::runtime_error(msg);
        }
        if (!tools::isclose(unit_handle->total_item_points(), unit_total_pts)) {
            std::string msg = "Army file: " + filename + " is corrupted. Mismatch in total item points values"
                                " in unit: " + unit_handle->name() + " of " +
                                tools::points_str(unit_handle->total_item_points() - unit_total_pts) + " pts";
            throw std::runtime_error(msg);
        }
        if (!tools::isclose(unit_handle->points(), unit_pts)) {
            std::string msg = "Army file: " + filename + " is corrupted. Mismatch in points values"
                                " in unit: " + unit_handle->name() + " of " +
                                tools::points_str(unit_handle->points() - unit_pts) + " pts";
            throw std::runtime_error(msg);
        }
    }

    void army_parser::parse_unit_assigned_name(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_handle->assign_name(s);
    }

    void army_parser::parse_unit_id(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        int id = std::stoi(s);
        if (ids.count(id)) {
            std::string msg = "Army file: " + filename + " is corrupted. Duplicate ID: " + s
                                + " in file";
            throw std::runtime_error(msg);
        }
        ids.insert(id);
        unit_handle->set_id(std::stoi(s));
    }

    void army_parser::parse_weapon(const std::string& s, bool champion, bool master, std::string type) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in " +
                                type + " field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in " +
                                type + " field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_weapon(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_melee_weapon(const std::string& s, bool champion, bool master) {
        parse_weapon(s, champion, master, "MELEE");
    }

    void army_parser::parse_unit_ranged_weapon(const std::string& s, bool champion, bool master) {
        parse_weapon(s, champion, master, "RANGED");
    }

    void army_parser::parse_armour(const std::string& s, bool champion, bool master, std::string type) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in " +
                                type + " field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in " +
                                type + " field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_armour(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_body_armour(const std::string& s, bool champion, bool master) {
        parse_armour(s, champion, master, "BODY");
    }

    void army_parser::parse_unit_shield_armour(const std::string& s, bool champion, bool master) {
        parse_armour(s, champion, master, "SHIELD");
    }

    void army_parser::parse_unit_helmet_armour(const std::string& s, bool champion, bool master) {
        parse_armour(s, champion, master, "HELMET");
    }

    void army_parser::parse_unit_oco_extra(const std::string& s, bool champion, bool master) {
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_oco_extra(s);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_mc_extras(const std::string& s, bool champion, bool master) {
        auto extras = tools::split(s, ';');
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        for (const auto& extra : extras) unit_handle->pick_mc_extra(extra);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_talisman(const std::string& s, bool champion, bool master) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in "
                                "TALISMAN field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in "
                                "TALISMAN field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_talisman(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_enchanted_item(const std::string& s, bool champion, bool master) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in "
                                "ENCHANTED_ITEM field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in "
                                "ENCHANTED_ITEM field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_enchanted_item(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_arcane_item(const std::string& s, bool champion, bool master) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in "
                                "ARCANE_ITEM field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in "
                                "ARCANE_ITEM field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_arcane_item(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_item_extras(const std::string& s, bool champion, bool master) {
        auto args = tools::split(s, ';');
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        for (const auto& arg : args) {
            auto item_args = tools::split(arg, ',');
            auto argname_value_pairs = tools::zip_args_to_names_values(item_args);
            auto search_name = std::find_if(
                std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                    return p.first == "name";
            });
            auto search_category = std::find_if(
                std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                    return p.first == "category";
            });
            if (search_name == std::end(argname_value_pairs)) {
                std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in "
                                    "MAGIC_ITEM_EXTRAS field of unit: " + unit_handle->name();
                throw std::runtime_error(msg);
            }
            if (search_category == std::end(argname_value_pairs)) {
                std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in "
                                    "MAGIC_ITEM_EXTRAS field of unit: " + unit_handle->name();
                throw std::runtime_error(msg);
            }

            unit_handle->pick_magic_extra(
                        enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second),
                        search_name->second
            );
        }
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_banner(const std::string& s, bool champion, bool master) {
        auto args = tools::split(s, ',');
        auto argname_value_pairs = tools::zip_args_to_names_values(args);
        auto search_name = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "name";
        });
        auto search_category = std::find_if(
            std::begin(argname_value_pairs), std::end(argname_value_pairs), [](const auto& p) {
                return p.first == "category";
        });
        if (search_name == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find name field in "
                                "BANNER field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (search_category == std::end(argname_value_pairs)) {
            std::string msg = "Army file: " + filename + " is corrupted. Unable to find category field in "
                                "BANNER field of unit: " + unit_handle->name();
            throw std::runtime_error(msg);
        }
        if (champion) unit_handle->switch_model_select(ModelSelect::CHAMPION);
        if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
        unit_handle->pick_banner(enum_convert::STRING_TO_ITEM_CLASS.at(search_category->second), search_name->second);
        unit_handle->switch_model_select(ModelSelect::DEFAULT);
        unit_handle->switch_mixed_select(MixedSelect::MASTER);
    }

    void army_parser::parse_unit_mount(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_handle->pick_mount(s);
    }

    void army_parser::parse_unit_mount_oco_option(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_handle->pick_mount_option(s, true);
    }

    void army_parser::parse_unit_mount_mc_options(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto extras = tools::split(s, ';');
        for (const auto& extra : extras) unit_handle->pick_mount_option(extra, false);
    }

    void army_parser::parse_unit_command(const std::string& s, bool champion, bool master) {
        auto members = tools::split(s, ';');
        // hacky way of casting to correct unit type --- use champion == true as mixed_unit case
        if (champion) {
            auto p = std::dynamic_pointer_cast<mixed_unit>(unit_handle);
            if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
            for (const auto& member : members)
                p->add_command_member(enum_convert::STRING_TO_COMMAND.at(member));
            unit_handle->switch_mixed_select(MixedSelect::MASTER);
        }
        else {
            auto p = std::dynamic_pointer_cast<normal_unit>(unit_handle);
            for (const auto& member : members)
                p->add_command_member(enum_convert::STRING_TO_COMMAND.at(member));
        }
    }

    void army_parser::parse_unit_size(const std::string& s, bool champion, bool master) {
        // hacky way of casting to correct unit type --- use champion == true as mixed_unit case
        if (champion) {
            auto p = std::dynamic_pointer_cast<mixed_unit>(unit_handle);
            if (!master) unit_handle->switch_mixed_select(MixedSelect::SLAVE);
            try {
                p->change_size(std::stoul(s));
            } catch (const std::invalid_argument&) {
                if (master)
                    cached_mixed_unit_master_size = s;
            }
            unit_handle->switch_mixed_select(MixedSelect::MASTER);
        }
        else {
            auto p = std::dynamic_pointer_cast<normal_unit>(unit_handle);
            p->change_size(std::stoul(s));
        }
    }

    void army_parser::parse_unit_mage_level(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto p = std::dynamic_pointer_cast<mage_character_unit>(unit_handle);
        p->change_level(static_cast<short>(std::stoi(s)));
    }

    void army_parser::parse_unit_mage_lores(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        auto p = std::dynamic_pointer_cast<mage_character_unit>(unit_handle);
        auto lores = tools::split(s, ';');
        for (const auto& lore : lores) p->pick_lore(lore);
    }


    void army_parser::parse_unit_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_pts = std::stod(s);
    }
    void army_parser::parse_unit_magic_item_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_magic_pts = std::stod(s);
    }
    void army_parser::parse_unit_faction_item_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_faction_pts = std::stod(s);
    }
    void army_parser::parse_unit_total_item_points(const std::string& s, bool champion, bool master) {
        (void)champion; (void)master;
        unit_total_pts = std::stod(s);
    }

}
