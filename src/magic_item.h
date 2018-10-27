#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include "enums.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct item {
    ItemClass item_class;
    ItemType item_type;
    ArmourType armour_type;
    WeaponType weapon_type;
    std::string name;
    std::string description;
    double points;
    unsigned int limit;
    std::unordered_set<std::string> allowed_units;
    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > restrictions;
};

#endif // !MAGIC_ITEM_H
