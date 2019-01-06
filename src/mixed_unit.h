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
public:
    explicit mixed_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
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

    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons() const noexcept override;
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour() const noexcept override;
    const std::pair<
        std::string,
        std::pair<bool, double>
    >& oco_extra() const noexcept override;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& mc_extras() const noexcept override;
    const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    double magic_item_points() const noexcept override;
    double faction_item_points() const noexcept override;
    double total_item_points() const noexcept override;

    std::string pick_weapon(ItemCategory item_type, const std::string& name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;
    std::string pick_armour(ItemCategory item_type, const std::string& name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;
    std::string pick_oco_extra(const std::string& name) override;
    std::string remove_oco_extra() override;
    std::string pick_mc_extra(const std::string& name) override;
    std::string remove_mc_extra(const std::string& name) override;
    void pick_mount(const std::string& name) override;
    void remove_mount() override;
    std::string pick_banner(ItemCategory item_class, const std::string& name) override;
    std::string remove_banner() override;

    void change_size(std::size_t n);
    void change_master_size(std::size_t n);
    void change_slave_size(std::size_t n);

    std::string html_table_row() const override;

    virtual std::string save() const override;
};

#endif // !MIXED_UNIT_H
