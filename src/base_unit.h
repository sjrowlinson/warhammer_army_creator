#ifndef BASE_UNIT_H
#define BASE_UNIT_H

#include "enums.h"
#include "magic_item.h"
#include "mount.h"
#include "options.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class base_unit {
protected:
    BaseUnitType but;
private:
    // enums
    Faction faction_;
    UnitType ut_;
    UnitCategory uc_;
    // basic
    std::string name_;
    // budgets
    budget magic_item_budget_;
    budget faction_item_budget_;
    budget total_item_budget_;
    double magic_banner_budget_;
    // other
    std::size_t min_size_;
    std::size_t max_size_;
    // handle to mounts
    std::shared_ptr<
        std::unordered_map<
            std::string, mount
        >
    > mounts;
    // handle to magic items
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > magic_items;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > common_items;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > faction_items;
public:
    explicit base_unit(
        Faction faction,
        UnitType ut,
        UnitCategory uc,
        const std::string& name,
        const budget& mi_budget,
        const budget& fi_budget,
        const budget& ti_budget,
        std::size_t min_size,
        std::size_t max_size = std::numeric_limits<std::size_t>::max(),
        double banner_budget = std::numeric_limits<double>::max()
    );
    virtual ~base_unit() = default;

    BaseUnitType base_unit_type() const noexcept;

    Faction faction() const noexcept;
    UnitType unit_type() const noexcept;
    UnitCategory unit_class() const noexcept;

    const std::string& name() const noexcept;
    std::size_t min_size() const noexcept;
    std::size_t max_size() const noexcept;

    const budget& magic_item_budget() const noexcept;
    const budget& faction_item_budget() const noexcept;
    const budget& total_item_budget() const noexcept;
    double magic_banner_budget() const noexcept;

    const std::shared_ptr<
        std::unordered_map<
            std::string, mount
        >
    >& mounts_handle() const noexcept;
    void set_mounts_handle(
        const std::shared_ptr<
           std::unordered_map<
               std::string, mount
           >
        >& mts
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& magic_items_handle() const noexcept;
    void set_magic_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& common_items_handle() const noexcept;
    void set_common_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& faction_items_handle() const noexcept;
    void set_faction_item_handle(
        const std::shared_ptr<
            std::pair<
                std::string,
                std::unordered_map<std::string, item>
            >
        >& items
    ) noexcept;
};

#endif // !BASE_UNIT_H
