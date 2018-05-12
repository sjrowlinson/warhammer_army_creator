#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H
#include "army_maps.h"
#include "unit.h"
#include "lord.h"
#include "hero.h"
#include "core.h"
#include "special.h"
#include "rare.h"

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
    //
public:
    selection_tree(armies::Faction faction);
    ~selection_tree();
};

#endif // !SELECTION_TREE_H