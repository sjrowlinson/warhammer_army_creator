#include "item.h"
#include "../enums.h"

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class weapon : public item {
private:
    WeaponType weapon_type_;
public:
    explicit weapon(
        WeaponType wt,
        ItemCategory ic,
        const std::string& nm,
        const std::string& descr,
        double pts,
        std::unordered_set<std::string> allowed,
        std::unordered_multimap<RestrictionField, std::any> restr,
        std::vector<std::string> rns
    );

    WeaponType weapon_type() const noexcept;
};
