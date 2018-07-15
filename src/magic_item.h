#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include <string>
#include <unordered_set>

enum class ItemClass {
    MUNDANE,
    MAGIC,
    FACTION,
    COMMON
};

enum class ItemType {
    WEAPON,
    ARMOUR,
    TALISMAN,
    ARCANE,
    ENCHANTED,
    BANNER
};

enum class WeaponType {
    MELEE,
    BALLISTIC
};

enum class ArmourType {
    ARMOUR,
    SHIELD,
    HELMET,
    NONE
};

struct magic_item {
    ItemType item_type;
    ArmourType armour_type;
    std::string name;
    std::string description;
    double points;
    std::unordered_set<std::string> allowed_units;
};

#endif // !MAGIC_ITEM_H
