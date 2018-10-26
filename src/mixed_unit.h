#ifndef MIXED_UNIT_H
#define MIXED_UNIT_H

#include "base_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "normal_unit.h"
#include "unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

class mixed_unit : public unit {
public:
    // handle
    const std::shared_ptr<base_mixed_unit> handle;
private:
    normal_unit master_;
    normal_unit slave_;

    std::size_t master_size_;
    std::size_t slaves_size_;
protected:
    std::pair<bool, std::string> restriction_check(
        RestrictionField picking,
        const std::unordered_map<RestrictionField, std::vector<std::string>>& restrictions
    ) const override;
public:
    explicit mixed_unit(std::shared_ptr<base_unit> base);
    mixed_unit(const mixed_unit& other);
    ~mixed_unit() override = default;

    bool is_character() const noexcept override;
    bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;

    bool switch_model_select(ModelSelect ms) override;
    bool switch_mixed_select(MixedSelect ms) override;

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
    const std::tuple<
        mount,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    std::pair<std::string, std::pair<ItemClass, double>> magic_banner() const noexcept override;

    std::string pick_weapon(ItemClass item_type, std::string name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;
    std::string pick_armour(ItemClass item_type, std::string name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;
    std::string pick_oco_extra(std::string name) override;
    std::string remove_oco_extra() override;
    std::string pick_mc_extra(std::string name) override;
    std::string remove_mc_extra(std::string name) override;
    void pick_mount(std::string name) override;
    void remove_mount() override;
    std::string pick_banner(ItemClass item_class, std::string name) override;
    std::string remove_banner() override;

    void change_size(std::size_t n);
    void change_master_size(std::size_t n);
    void change_slave_size(std::size_t n);

    std::string html_table_row() const override;
};

#endif // !MIXED_UNIT_H
