#include "army_maps.h"

namespace armies {
    void armies::parse_roster_file(
        armies::Faction faction,
        std::shared_ptr<
            std::unordered_map<std::string, armies::unit_properties>
        >& army_map
    ) {
        std::string filename;
        switch (faction) {
            case armies::Faction::EMPIRE:
                filename = "empire_roster.ros";
                break;
            case armies::Faction::BRETONNIA:
                filename = "bretonnia_roster.ros";
                break;
            case armies::Faction::DWARFS:
                filename = "dwarfs_roster.ros";
                break;
            case armies::Faction::HIGH_ELVES:
                filename = "high_elves_roster.ros";
                break;
            // ...
            case armies::Faction::SKAVEN:
                filename = "skaven_roster.ros";
                break;
        }
        auto tmp = new std::unordered_map<
            std::string, unit_properties
        >();
        // TODO: parse the file into tmp
        army_map.reset(tmp);
    }
};