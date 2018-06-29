#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include <string>
#include <vector>

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
    std::vector<std::string> allowed_units;
};

#endif // !MAGIC_ITEM_H