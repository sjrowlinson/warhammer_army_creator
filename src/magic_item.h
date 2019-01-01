#ifndef MAGIC_ITEM_H
#define MAGIC_ITEM_H

#include "enums.h"

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct item {
    ItemCategory item_class;
    ItemType item_type;
    ArmourType armour_type;
    WeaponType weapon_type;
    std::string name;
    std::string description;
    double points;
    unsigned int limit;
    std::unordered_set<std::string> allowed_units;
    // TODO: change to std::unordered_multimap where duplicate keys
    // indicate OR options - e.g. the Third Eye of Tzeentch WoC
    // faction item requires either the Mark of Khorne or Daemon
    // of Khorne {oco_extra}
    std::unordered_multimap<RestrictionField, std::any> restrictions;
};

// TODO: change restrictions to a std::unordered_multimap<RestrictionField, std::any>
//       so that I can do different things based on the field of restriction stored
//       => e.g: RestrictionField::LIMIT can just be handled with a value_type of int

#endif // !MAGIC_ITEM_H
