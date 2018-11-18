#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H

#include "army_list.h"
#include "base_unit.h"
#include "base_character_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "enums.h"
#include "item_parser.h"
#include "mage_character_unit.h"
#include "magic_item.h"
#include "melee_character_unit.h"
#include "mixed_unit.h"
#include "mounts_parser.h"
#include "normal_unit.h"
#include "roster_parser.h"
#include "tools.h"
#include "unit.h"

#include <cmath>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QString>

#include <iostream>

class selection_tree {
private:
    Faction race;
    // roster of units
    std::unordered_map<
        std::string,
        std::shared_ptr<base_unit>
    > roster;
    // faction mounts
    std::unordered_map<
        std::string,
        mount
    > mounts;
    // magic and faction items
    std::pair<
        std::string,
        std::unordered_map<
            std::string,
            item
        >
    > magic_items;
    std::pair<
        std::string,
        std::unordered_map<
            std::string,
            item
        >
    > common_items;
    std::pair<
        std::string,
        std::unordered_map<
            std::string,
            item
        >
    > faction_items;

    std::shared_ptr<unit> current_selection;

    std::reference_wrapper<army_list> army;

    std::tuple<std::string, std::string, std::string, std::string> filenames() const noexcept;
    void parse_roster_file(const QString& rfile_str);
    void parse_mount_file(const QString& mfile_str);
    void parse_item_files(const std::pair<QString, QString>& ifile_str);
    std::vector<std::shared_ptr<base_unit>> all_of(UnitType ut) const noexcept;
public:
    explicit selection_tree(Faction faction, army_list& list);
    ~selection_tree() = default;
    void add_unit_to_army_list(int id);
    void change_selection(const std::string& name);
    void reset_army_list(army_list&_army);
    void reset(Faction faction, army_list& list);

    std::shared_ptr<unit> selected();

    std::string magic_items_name() const noexcept;
    std::string faction_items_name() const noexcept;

    std::vector<std::shared_ptr<base_unit>> lords() const noexcept;
    std::vector<std::shared_ptr<base_unit>> heroes() const noexcept;
    std::vector<std::shared_ptr<base_unit>> core() const noexcept;
    std::vector<std::shared_ptr<base_unit>> special() const noexcept;
    std::vector<std::shared_ptr<base_unit>> rare() const noexcept;

};

#endif // !SELECTION_TREE_H
