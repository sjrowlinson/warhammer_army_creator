#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H

#include "army_maps.h"
#include "army_list.h"
#include "base_unit.h"
#include "item_parser.h"
#include "magic_item.h"
#include "roster_parser.h"
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
    armies::Faction race;
    std::unordered_map<
        std::string,
        std::shared_ptr<base_unit>
    > roster;
    std::unordered_map<
        std::string,
        magic_item
    > magic_items;
    unit current_selection;

    std::reference_wrapper<army_list> army;

    void parse_roster_file(const std::string& roster_file);
    void parse_item_file(const std::string& item_file);
public:
    selection_tree(armies::Faction faction, army_list& list);
    ~selection_tree() = default;
    void add_unit_to_army_list();
    void change_selection(const std::string& name);
    void reset_army_list(army_list&_army);
    unit& selected();
};

#endif // !SELECTION_TREE_H