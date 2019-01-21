#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include "enums.h"

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// TODO: add runes field (something like std::unordered_multimap<RuneType, std::string>)

struct item {
    ItemCategory item_class;
    ItemType item_type;
    ArmourType armour_type;
    WeaponType weapon_type;
    std::string name;
    std::string description;
    double points;
    std::unordered_set<std::string> allowed_units;
    std::unordered_multimap<RestrictionField, std::any> restrictions;
};

#endif // !MAGIC_ITEM_H
