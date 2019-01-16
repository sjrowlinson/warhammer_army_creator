#include "enums.h"

namespace enum_convert {
    bool in_army_trees(InTree in_tree) {
        return in_tree == InTree::LORD || in_tree == InTree::HERO
                || in_tree == InTree::CORE || in_tree == InTree::SPECIAL
                || in_tree == InTree::RARE;
    }
}
