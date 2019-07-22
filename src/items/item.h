#ifndef ITEM_H
#define ITEM_H

#include "../enums.h"

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class item {
private:
    ItemCategory item_category_;
    ItemType item_type_;
    std::string name_;
    std::string description_;
    double points_;
    std::unordered_set<std::string> allowed_units_;
    std::unordered_multimap<RestrictionField, std::any> restrictions_;
    std::vector<std::string> runes_;
public:
    explicit item(
        ItemCategory ic,
        ItemType it,
        const std::string& nm,
        const std::string& descr,
        double pts,
        std::unordered_set<std::string> allowed,
        std::unordered_multimap<RestrictionField, std::any> restr,
        std::vector<std::string> rns
    );

    ItemCategory category() const noexcept;
    ItemType type() const noexcept;
    const std::string& name() const noexcept;
    const std::string& description() const noexcept;
    double points() const noexcept;
    const std::unordered_set<std::string>& allowed_units() const noexcept;
    const std::unordered_multimap<RestrictionField, std::any>& restrictions() const noexcept;
};

#endif // !ITEM_H