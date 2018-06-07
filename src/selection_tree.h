#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H
#include "army_maps.h"
#include "army_list.h"
#include "file_reader.h"
#include "tools.h"
#include "unit.h"
#include <array>
#include <cmath>
#include <memory>
#include <string>

/**
 * This class represents the left-box of the interface displaying
 * the army roster for the selected race. The user clicks on units
 * in this tree and can add them to their army list - the unit will
 * be copied to an intermediate stage and a std::shared_ptr will
 * be made from the copy in order to add it to the army_list structure.
 *
 * The shared_ptr of the copy made in the intermediate stage will be
 * the object manipulated by the user in the army list view (centre
 * box) - such that any changes made here are copied across to the
 * army_list structure [TODO: but how will these changes update the
 * points value in army_list? some sort of event firer?]
 */
class selection_tree {
private:
    std::unordered_map<
        std::string,
        unit
    > roster;
    armies::Faction race;
    std::shared_ptr<unit> current_selection;

    void parse_roster_file(const std::string& filename);
    std::vector<
        std::pair<std::string, std::string>
    > parse_table(const std::string& table);
    void parse_stat_table(unit& temp, const std::string& table);
    void parse_equipment_table(unit& temp, const std::string& table);
    void parse_special_rules_table(unit& temp, const std::string& table);
    void parse_options_table(unit& temp, const std::string& table);
public:
    selection_tree(armies::Faction faction);
    ~selection_tree() = default;
    void add_unit_to_army_list(army_list& list);
    void change_selection(const std::string& name);
};

#endif // !SELECTION_TREE_H