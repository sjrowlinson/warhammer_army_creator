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
    BANNER,
    OTHER
};

enum class WeaponType {
    MELEE,
    BALLISTIC,
    NONE
};

enum class ArmourType {
    ARMOUR,
    SHIELD,
    HELMET,
    NONE
};

struct item {
    ItemClass item_class;
    ItemType item_type;
    ArmourType armour_type;
    WeaponType weapon_type;
    std::string name;
    std::string description;
    double points;
    std::unordered_set<std::string> allowed_units;
};

#endif // !MAGIC_ITEM_H
