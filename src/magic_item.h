#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include <string>

enum class MagicItemType {
    WEAPON,
    ARMOUR,
    TALISMAN,
    ARCANE,
    ENCHANTED,
    BANNER
};

struct magic_item {
    MagicItemType item_type;
    std::string name;
    std::string description;
    double points;
};

#endif // !MAGIC_ITEM_H