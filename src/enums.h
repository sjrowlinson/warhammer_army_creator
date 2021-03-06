#ifndef ENUMS_H
#define ENUMS_H
#include <string>
#include <unordered_map>

// army name

enum class Faction {
    EMPIRE,
    BRETONNIA,
    DWARFS,
    HIGH_ELVES,
    WOOD_ELVES,
    DARK_ELVES,
    WARRIORS_OF_CHAOS,
    DAEMONS_OF_CHAOS,
    BEASTMEN,
    CHAOS_DWARFS,
    VAMPIRE_COUNTS,
    TOMB_KINGS,
    OGRE_KINGDOMS,
    SKAVEN,
    ORCS_AND_GOBLINS,
    NONE
};

// type and category of unit

enum class UnitType {
    LORD,
    HERO,
    CORE,
    SPECIAL,
    RARE,
    NONE
};

enum class UnitCategory {
    INFANTRY,
    CAVALRY,
    MONSTROUS_INFANTRY,
    MONSTROUS_CAVALRY,
    MONSTROUS_BEAST,
    WARBEASTS,
    CHARIOT,
    MONSTER,
    SWARM,
    WARMACHINE,
    UNIQUE
};

// base unit field related

enum class BaseUnitType {
    NORMAL,
    MIXED,
    MELEE_CHARACTER,
    MAGE_CHARACTER,
    BASE
};

// unit field related

enum class CommandGroup {
    MUSICIAN,
    STANDARD_BEARER,
    CHAMPION
};

enum class RestrictionField {
    WEAPON,
    ARMOUR,
    TALISMAN,
    ENCHANTED,
    ARCANE,
    BANNER,
    MOUNT,
    OCO_EXTRA,
    MC_EXTRA,
    OTHER,
    ARMY_CONTAINS,
    LIMIT,
    ITEMTYPE, // -> for budget restrictions
    SUBITEMTYPE,
    LORE
};

enum class ModelSelect {
    DEFAULT,
    CHAMPION
};

enum class MixedSelect {
    MASTER,
    SLAVE
};

// item types

enum class ItemCategory {
    MUNDANE,
    MAGIC,
    FACTION,
    COMMON,
    NONE
};

enum class ItemType {
    WEAPON,
    ARMOUR,
    TALISMAN,
    ARCANE,
    ENCHANTED,
    BANNER,
    OTHER,
    NONE
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

enum class BudgetType {
    MAGIC,
    FACTION,
    TOTAL,
    NONE
};

// army list invalidity reasons

enum class InvalidListReason {
    POINTS,
    LORD_LIMIT,
    HERO_LIMIT,
    CORE_MINIMUM,
    SPECIAL_LIMIT,
    RARE_LIMIT
};

// GUI related

enum class InTree {
    ROSTER,
    ARMY,
    LORD,
    HERO,
    CORE,
    SPECIAL,
    RARE,
    NEITHER
};

// TODO: split into two different - one for characters, other for non-character units
enum class ArmyTreeColumn {
    NAME,
    MOUNT,
    SIZE,
    LEVEL,
    LORE,
    WEAPONS,
    ARMOUR,
    TALISMAN,
    ENCHANTED,
    ARCANE,
    COMMAND,
    BANNER,
    EXTRAS,
    POINTS,
    ALL
};

enum class CharacterTreeColumn {
    NAME,
    MOUNT,
    LEVEL,
    LORE,
    WEAPONS,
    ARMOUR,
    TALISMAN,
    ENCHANTED,
    ARCANE,
    BANNER,
    EXTRAS,
    POINTS,
    ALL
};

enum class UnitTreeColumn {
    NAME,
    SIZE,
    MOUNT,
    WEAPONS,
    ARMOUR,
    EXTRAS,
    COMMAND,
    BANNER,
    POINTS,
    ALL
};

namespace enum_convert {
    bool in_army_trees(InTree in_tree);

    const std::unordered_map<
        ArmyTreeColumn, CharacterTreeColumn
    > ATC_TO_CTC = {
        {ArmyTreeColumn::NAME, CharacterTreeColumn::NAME},
        {ArmyTreeColumn::MOUNT, CharacterTreeColumn::MOUNT},
        {ArmyTreeColumn::LEVEL, CharacterTreeColumn::LEVEL},
        {ArmyTreeColumn::LORE, CharacterTreeColumn::LORE},
        {ArmyTreeColumn::WEAPONS, CharacterTreeColumn::WEAPONS},
        {ArmyTreeColumn::ARMOUR, CharacterTreeColumn::ARMOUR},
        {ArmyTreeColumn::TALISMAN, CharacterTreeColumn::TALISMAN},
        {ArmyTreeColumn::ENCHANTED, CharacterTreeColumn::ENCHANTED},
        {ArmyTreeColumn::ARCANE, CharacterTreeColumn::ARCANE},
        {ArmyTreeColumn::BANNER, CharacterTreeColumn::BANNER},
        {ArmyTreeColumn::EXTRAS, CharacterTreeColumn::EXTRAS},
        {ArmyTreeColumn::POINTS, CharacterTreeColumn::POINTS},
        {ArmyTreeColumn::ALL, CharacterTreeColumn::ALL}
    };

    const std::unordered_map<
        ArmyTreeColumn, UnitTreeColumn
    > ATC_TO_UTC = {
        {ArmyTreeColumn::NAME, UnitTreeColumn::NAME},
        {ArmyTreeColumn::MOUNT, UnitTreeColumn::MOUNT},
        {ArmyTreeColumn::SIZE, UnitTreeColumn::SIZE},
        {ArmyTreeColumn::WEAPONS, UnitTreeColumn::WEAPONS},
        {ArmyTreeColumn::ARMOUR, UnitTreeColumn::ARMOUR},
        {ArmyTreeColumn::BANNER, UnitTreeColumn::BANNER},
        {ArmyTreeColumn::EXTRAS, UnitTreeColumn::EXTRAS},
        {ArmyTreeColumn::COMMAND, UnitTreeColumn::COMMAND},
        {ArmyTreeColumn::POINTS, UnitTreeColumn::POINTS},
        {ArmyTreeColumn::ALL, UnitTreeColumn::ALL}
    };

    const std::unordered_map<
        std::string, Faction
    > STRING_TO_FACTION = {
        {"The Empire", Faction::EMPIRE},
        {"Bretonnia", Faction::BRETONNIA},
        {"Dwarfs", Faction::DWARFS},
        {"High Elves", Faction::HIGH_ELVES},
        {"Dark Elves", Faction::DARK_ELVES},
        {"Wood Elves", Faction::WOOD_ELVES},
        {"Warriors of Chaos", Faction::WARRIORS_OF_CHAOS},
        {"Daemons of Chaos", Faction::DAEMONS_OF_CHAOS},
        {"Beastmen", Faction::BEASTMEN},
        {"Chaos Dwarfs", Faction::CHAOS_DWARFS},
        {"Vampire Counts", Faction::VAMPIRE_COUNTS},
        {"Tomb Kings", Faction::TOMB_KINGS},
        {"Ogre Kingdoms", Faction::OGRE_KINGDOMS},
        {"Skaven", Faction::SKAVEN},
        {"Orcs and Goblins", Faction::ORCS_AND_GOBLINS}
    };
    const std::unordered_map<
        Faction, std::string
    > FACTION_TO_STRING = {
        {Faction::EMPIRE, "The Empire"},
        {Faction::BRETONNIA, "Bretonnia"},
        {Faction::DWARFS, "Dwarfs"},
        {Faction::HIGH_ELVES, "High Elves"},
        {Faction::DARK_ELVES, "Dark Elves"},
        {Faction::WOOD_ELVES, "Wood Elves"},
        {Faction::WARRIORS_OF_CHAOS, "Warriors of Chaos"},
        {Faction::DAEMONS_OF_CHAOS, "Daemons of Chaos"},
        {Faction::BEASTMEN, "Beastmen"},
        {Faction::CHAOS_DWARFS, "Chaos Dwarfs"},
        {Faction::VAMPIRE_COUNTS, "Vampire Counts"},
        {Faction::TOMB_KINGS, "Tomb Kings"},
        {Faction::OGRE_KINGDOMS, "Ogre Kingdoms"},
        {Faction::SKAVEN, "Skaven"},
        {Faction::ORCS_AND_GOBLINS, "Orcs and Goblins"}
    };


    const std::unordered_map<
        std::string, UnitType
    > STRING_TO_UNIT_TYPE = {
        {"Lord", UnitType::LORD},
        {"Hero", UnitType::HERO},
        {"Core", UnitType::CORE},
        {"Special", UnitType::SPECIAL},
        {"Rare", UnitType::RARE}
    };
    const std::unordered_map<
        UnitType, std::string
    > UNIT_TYPE_TO_STRING = {
        {UnitType::LORD, "Lord"},
        {UnitType::HERO, "Hero"},
        {UnitType::CORE, "Core"},
        {UnitType::SPECIAL, "Special"},
        {UnitType::RARE, "Rare"}
    };

    const std::unordered_map<
        std::string, UnitCategory
    > STRING_TO_UNIT_CLASS = {
        {"Infantry", UnitCategory::INFANTRY},
        {"Cavalry", UnitCategory::CAVALRY},
        {"Monstrous Infantry", UnitCategory::MONSTROUS_INFANTRY},
        {"Monstrous Cavalry", UnitCategory::MONSTROUS_CAVALRY},
        {"Monstrous Beast", UnitCategory::MONSTROUS_BEAST},
        {"Warbeast", UnitCategory::WARBEASTS},
        {"Chariot", UnitCategory::CHARIOT},
        {"Monster", UnitCategory::MONSTER},
        {"Swarm", UnitCategory::SWARM},
        {"Warmachine", UnitCategory::WARMACHINE},
        {"Unique", UnitCategory::UNIQUE}
    };
    const std::unordered_map<
        UnitCategory, std::string
    > UNIT_CLASS_TO_STRING = {
        {UnitCategory::INFANTRY, "Infantry"},
        {UnitCategory::CAVALRY, "Cavalry"},
        {UnitCategory::MONSTROUS_INFANTRY, "Monstrous Infantry"},
        {UnitCategory::MONSTROUS_CAVALRY, "Monstrous Cavalry"},
        {UnitCategory::MONSTROUS_BEAST, "Monstrous Beast"},
        {UnitCategory::WARBEASTS, "Warbeast"},
        {UnitCategory::CHARIOT, "Chariot"},
        {UnitCategory::MONSTER, "Monster"},
        {UnitCategory::SWARM, "Swarm"},
        {UnitCategory::WARMACHINE, "Warmachine"},
        {UnitCategory::UNIQUE, "Unique"}
    };

    const std::unordered_map<
        std::string, BaseUnitType
    > STRING_TO_BASE_UNIT_TYPE = {
        {"Melee", BaseUnitType::MELEE_CHARACTER},
        {"Mage", BaseUnitType::MAGE_CHARACTER},
        {"Normal", BaseUnitType::NORMAL},
        {"Mixed", BaseUnitType::MIXED}
    };

    const std::unordered_map<
        std::string, RestrictionField
    > STRING_TO_RESTRICTION = {
        {"Weapon", RestrictionField::WEAPON},
        {"Armour", RestrictionField::ARMOUR},
        {"Talisman", RestrictionField::TALISMAN},
        {"Enchanted", RestrictionField::ENCHANTED},
        {"Arcane", RestrictionField::ARCANE},
        {"Banner", RestrictionField::BANNER},
        {"Mount", RestrictionField::MOUNT},
        {"oco_extra", RestrictionField::OCO_EXTRA},
        {"mc_extra", RestrictionField::MC_EXTRA},
        {"other", RestrictionField::OTHER},
        {"ArmyContains", RestrictionField::ARMY_CONTAINS},
        {"Limit", RestrictionField::LIMIT},
        {"ItemType", RestrictionField::ITEMTYPE},
        {"SubItemType", RestrictionField::SUBITEMTYPE},
        {"Lore", RestrictionField::LORE}
    };
    const std::unordered_map<
        RestrictionField, std::string
    > RESTRICTION_TO_STRING = {
        {RestrictionField::WEAPON, "Weapon"},
        {RestrictionField::ARMOUR, "Armour"},
        {RestrictionField::TALISMAN, "Talisman"},
        {RestrictionField::ENCHANTED, "Enchanted"},
        {RestrictionField::ARCANE, "Arcane"},
        {RestrictionField::BANNER, "Banner"},
        {RestrictionField::MOUNT, "Mount"},
        {RestrictionField::OCO_EXTRA, "oco_extra"},
        {RestrictionField::MC_EXTRA, "mc_extra"},
        {RestrictionField::OTHER, "other"},
        {RestrictionField::ARMY_CONTAINS, "ArmyContains"},
        {RestrictionField::LIMIT, "Limit"},
        {RestrictionField::ITEMTYPE, "ItemType"},
        {RestrictionField::SUBITEMTYPE, "SubItemType"},
        {RestrictionField::LORE, "Lore"}
    };

    const std::unordered_map<
        std::string, CommandGroup
    > STRING_TO_COMMAND = {
        {"Musician", CommandGroup::MUSICIAN},
        {"Standard Bearer", CommandGroup::STANDARD_BEARER},
        {"Champion", CommandGroup::CHAMPION}
    };
    const std::unordered_map<
        CommandGroup, std::string
    > COMMAND_TO_STRING = {
        {CommandGroup::MUSICIAN, "Musician"},
        {CommandGroup::STANDARD_BEARER, "Standard Bearer"},
        {CommandGroup::CHAMPION, "Champion"}
    };

    const std::unordered_map<
        std::string, ItemCategory
    > STRING_TO_ITEM_CLASS = {
        {"Mundane", ItemCategory::MUNDANE},
        {"Magic", ItemCategory::MAGIC},
        {"Faction", ItemCategory::FACTION},
        {"Common", ItemCategory::COMMON}
    };
    const std::unordered_map<
        ItemCategory, std::string
    > ITEM_CLASS_TO_STRING = {
        {ItemCategory::MUNDANE, "Mundane"},
        {ItemCategory::MAGIC, "Magic"},
        {ItemCategory::FACTION, "Faction"},
        {ItemCategory::COMMON, "Common"}
    };

    const std::unordered_map<
        std::string, ItemType
    > STRING_TO_ITEM_TYPE = {
        {"Weapon", ItemType::WEAPON},
        {"Armour", ItemType::ARMOUR},
        {"Talisman", ItemType::TALISMAN},
        {"Arcane", ItemType::ARCANE},
        {"Enchanted", ItemType::ENCHANTED},
        {"Banner", ItemType::BANNER},
        {"Other", ItemType::OTHER}
    };
    const std::unordered_map<
        ItemType, std::string
    > ITEM_TYPE_TO_STRING = {
        {ItemType::WEAPON, "Weapon"},
        {ItemType::ARMOUR, "Armour"},
        {ItemType::TALISMAN, "Talisman"},
        {ItemType::ARCANE, "Arcane"},
        {ItemType::ENCHANTED, "Enchanted"},
        {ItemType::BANNER, "Banner"},
        {ItemType::OTHER, "Other"}
    };

    const std::unordered_map<
        std::string, WeaponType
    > STRING_TO_WEAPON_TYPE = {
        {"Melee", WeaponType::MELEE},
        {"Ballistic", WeaponType::BALLISTIC},
        {"None", WeaponType::NONE}
    };
    const std::unordered_map<
        WeaponType, std::string
    > WEAPON_TYPE_TO_STRING = {
        {WeaponType::MELEE, "Melee"},
        {WeaponType::BALLISTIC, "Ballistic"},
        {WeaponType::NONE, "None"}
    };

    const std::unordered_map<
        std::string, ArmourType
    > STRING_TO_ARMOUR_TYPE = {
        {"Armour", ArmourType::ARMOUR},
        {"Shield", ArmourType::SHIELD},
        {"Helmet", ArmourType::HELMET},
        {"None", ArmourType::NONE}
    };
    const std::unordered_map<
        ArmourType, std::string
    > ARMOUR_TYPE_TO_STRING = {
        {ArmourType::ARMOUR, "Armour"},
        {ArmourType::SHIELD, "Shield"},
        {ArmourType::HELMET, "Helmet"},
        {ArmourType::NONE, "None"}
    };
}

#endif // !ENUMS_H
