#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include "enums.h"

#include <string>
#include <unordered_set>

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
