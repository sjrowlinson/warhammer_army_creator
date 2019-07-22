#include "item.h"
#include "../enums.h"

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class armour : public item {
private:
    ArmourType armour_type_;
public:
    explicit armour(
        ArmourType at,
        ItemCategory ic,
        const std::string& nm,
        const std::string& descr,
        double pts,
        std::unordered_set<std::string> allowed,
        std::unordered_multimap<RestrictionField, std::any> restr,
        std::vector<std::string> rns
    );

    ArmourType armour_type() const noexcept;
};
