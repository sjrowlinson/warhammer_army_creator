#ifndef MIXED_UNIT_H
#define MIXED_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "normal_unit.h"
#include "unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

enum class MixedSelect {
    MASTER,
    SLAVE
};

class mixed_unit : public unit {
public:
    // handle
    const std::shared_ptr<base_mixed_unit> handle;
private:
    normal_unit master_;
    normal_unit slave_;

    MixedSelect mixed_select_;

    std::size_t master_size_;
    std::size_t slaves_size_;
public:
    explicit mixed_unit(std::shared_ptr<base_unit> base);
    ~mixed_unit() = default;

    normal_unit& master() noexcept;
    normal_unit& slave() noexcept;

    double points() const noexcept override;

    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons() const noexcept override;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept override;
    std::pair<
        std::string,
        std::pair<bool, double>
    > oco_extra() const noexcept override;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > mc_extras() const noexcept override;

    void pick_weapon(ItemClass item_type, std::string name) override;
    void remove_weapon(WeaponType wt) override;
    void pick_armour(ItemClass item_type, std::string name) override;
    void remove_armour(ArmourType at) override;
    void pick_oco_extra(std::string name) override;
    void remove_oco_extra() override;
    void pick_mc_extra(std::string name) override;
    void remove_mc_extra(std::string name) override;

};

#endif // !MIXED_UNIT_H
