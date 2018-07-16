#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H

#include "army_maps.h"
#include "army_list.h"
#include "base_unit.h"
#include "item_parser.h"
#include "magic_item.h"
//#include "roster_parser.h"
#include "experimental_parser.h"
#include "tools.h"
#include "unit.h"

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <QString>

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

    std::pair<std::string, std::string> filenames() const noexcept;
    void parse_roster_file(const QString& rfile_str);
    void parse_item_file(const QString& ifile_str);
    std::vector<std::shared_ptr<base_unit>> all_of(armies::UnitType ut) const noexcept;
public:
    selection_tree(armies::Faction faction, army_list& list);
    ~selection_tree() = default;
    void add_unit_to_army_list(int id);
    void change_selection(const std::string& name);
    void reset_army_list(army_list&_army);
    void reset(armies::Faction faction, army_list& list);

    unit& selected();

    std::vector<std::shared_ptr<base_unit>> lords() const noexcept;
    std::vector<std::shared_ptr<base_unit>> heroes() const noexcept;
    std::vector<std::shared_ptr<base_unit>> core() const noexcept;
    std::vector<std::shared_ptr<base_unit>> special() const noexcept;
    std::vector<std::shared_ptr<base_unit>> rare() const noexcept;

};

#endif // !SELECTION_TREE_H
