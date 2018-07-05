#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include <string>
#include <unordered_set>

enum class MagicItemType {
    WEAPON,
    ARMOUR,
    TALISMAN,
    ARCANE,
    ENCHANTED,
    BANNER
};

enum class ItemType {
    MUNDANE,
    MAGIC,
    FACTION,
    COMMON
};

enum class ArmourType {
    ARMOUR,
    SHIELD,
    HELMET,
};

struct magic_item {
    MagicItemType item_type;
    std::string name;
    std::string description;
    double points;
    std::unordered_set<std::string> allowed_units;
};

#endif // !MAGIC_ITEM_H