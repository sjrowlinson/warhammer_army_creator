#ifndef mixed_unitH
#define mixed_unitH

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
private:
    std::size_t master_size_;
    std::size_t slave_size_;

    // COMBINED
    std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;

    // MASTER
    // => non-champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > master_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > master_armours_;
    std::pair<std::string, std::pair<bool, double>> master_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> master_mc_extras_;
    // => champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > master_champ_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > master_champ_armours_;
    std::pair<std::string, std::pair<ItemCategory, double>> master_champ_talisman_;
    std::pair<std::string, std::pair<ItemCategory, double>> master_champ_enchanted_item_;
    std::pair<std::string, std::pair<ItemCategory, double>> master_champ_arcane_;
    std::unordered_map<std::string, std::pair<ItemCategory, double>> master_champ_item_extras_;
    std::pair<std::string, std::pair<bool, double>> master_champ_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> master_champ_mc_extras_;

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > master_command_group;
    std::pair<std::string, std::pair<ItemCategory, double>> banner;

    // SLAVE
    // => non-champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > slave_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > slave_armours_;
    std::pair<std::string, std::pair<bool, double>> slave_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> slave_mc_extras_;
    // => champion
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > slave_champion_weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > slave_champion_armours_;
    std::pair<std::string, std::pair<bool, double>> slave_champion_oco_extra_;
    std::unordered_map<std::string, std::pair<bool, double>> slave_champion_mc_extras_;

    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > slave_command_group;

    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons_access() noexcept override;
    std::unordered_map<
            ArmourType,
            std::tuple<ItemCategory, std::string, double>
    >& armour_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& talisman_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& enchanted_item_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item_access() noexcept override;
    std::unordered_map<
        std::string,
        std::pair<ItemCategory, double>
    >& item_extras_access() noexcept override;
    std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& magic_banner_acces() noexcept override;
public:
    const std::shared_ptr<base_mixed_unit> handle;

    explicit mixed_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
    mixed_unit(const mixed_unit& other);
    ~mixed_unit() override = default;

    bool is_character() const noexcept override;
    bool is_mage() const noexcept override;
    bool is_mixed() const noexcept override;

    bool switch_model_select(ModelSelect ms) override;
    bool switch_mixed_select(MixedSelect ms) override;

    std::size_t master_size() const noexcept;
    std::size_t slave_size() const noexcept;

    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& master_command() const noexcept;
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& slave_command() const noexcept;

    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& weapons() const noexcept override;

    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& slave_weapons() const noexcept;
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& slave_champion_weapons() const noexcept;
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& master_weapons() const noexcept;
    const std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    >& master_champion_weapons() const noexcept;

    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& armour() const noexcept override;

    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& slave_armour() const noexcept;
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& slave_champion_armour() const noexcept;
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& master_armour() const noexcept;
    const std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    >& master_champion_armour() const noexcept;

    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& talisman() const noexcept override;

    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& enchanted_item() const noexcept override;

    const std::pair<
        std::string,
        std::pair<ItemCategory, double>
    >& arcane_item() const noexcept override;

    const std::unordered_map<
        std::string,
        std::pair<ItemCategory, double>
    >& item_extras() const noexcept override;

    const std::pair<
        std::string,
        std::pair<bool, double>
    >& oco_extra() const noexcept override;

    const std::pair<
        std::string,
        std::pair<bool, double>
    >& slave_oco_extra() const noexcept;
    const std::pair<
        std::string,
        std::pair<bool, double>
    >& slave_champion_oco_extra() const noexcept;
    const std::pair<
        std::string,
        std::pair<bool, double>
    >& master_oco_extra() const noexcept;
    const std::pair<
        std::string,
        std::pair<bool, double>
    >& master_champion_oco_extra() const noexcept;

    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& mc_extras() const noexcept override;

    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& slave_mc_extras() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& slave_champion_mc_extras() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& master_mc_extras() const noexcept;
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& master_champion_mc_extras() const noexcept;

    const std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    >& mnt() const noexcept override;

    const std::pair<std::string, std::pair<ItemCategory, double>>& magic_banner() const noexcept override;

    // item selectors:

    // => weapon selection and removal
    std::string pick_weapon(ItemCategory item_category, const std::string& name) override;
    std::string remove_weapon(WeaponType wt, bool replacing=false) override;
    // ==> MASTER
    std::string pick_master_default_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_master_default_weapon(WeaponType wt, bool replacing=false);
    std::string pick_master_champion_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_master_champion_weapon(WeaponType wt, bool replacing=false);
    // ==> SLAVE
    std::string pick_slave_default_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_slave_default_weapon(WeaponType wt, bool replacing=false);
    std::string pick_slave_champion_weapon(ItemCategory item_category, const std::string& name);
    std::string remove_slave_champion_weapon(WeaponType wt, bool replacing=false);

    // => armour selection and removal
    std::string pick_armour(ItemCategory item_category, const std::string& name) override;
    std::string remove_armour(ArmourType at, bool replacing=false) override;
    // ==> MASTER
    std::string pick_master_default_armour(ItemCategory item_category, const std::string& name);
    std::string remove_master_default_armour(ArmourType at, bool replacing=false);
    std::string pick_master_champion_armour(ItemCategory item_category, const std::string& name);
    std::string remove_master_champion_armour(ArmourType at, bool replacing=false);
    // ==> SLAVE
    std::string pick_slave_default_armour(ItemCategory item_category, const std::string& name);
    std::string remove_slave_default_armour(ArmourType at, bool replacing=false);
    std::string pick_slave_champion_armour(ItemCategory item_category, const std::string& name);
    std::string remove_slave_champion_armour(ArmourType at, bool replacing=false);

    // => talisman selection and removal [NOTE: always refers to champion for non-characters]
    std::string pick_talisman(ItemCategory item_category, const std::string& name) override;
    std::string remove_talisman() override;

    // => enchanted item selection and removal [NOTE: always refers to champion for non-characters]
    std::string pick_enchanted_item(ItemCategory item_category, const std::string& name) override;
    std::string remove_enchanted_item() override;

    // => arcane item selection and removal [NOTE: always refers to champion for non-characters]
    std::string pick_arcane_item(ItemCategory item_category, const std::string& name) override;
    std::string remove_arcane_item() override;

    // => other magic item selection and removal [NOTE: always refers to champion for non-characters]
    std::string pick_magic_extra(ItemCategory item_category, const std::string& name) override;
    std::string remove_magic_extra(const std::string& name) override;

    // => one-choice-only extra selection and removal
    std::string pick_oco_extra(const std::string& name) override;
    std::string remove_oco_extra() override;
    // ==> MASTER
    std::string pick_master_default_oco_extra(const std::string& name);
    std::string remove_master_default_oco_extra();
    std::string pick_master_champion_oco_extra(const std::string& name);
    std::string remove_master_champion_oco_extra();
    // ==> SLAVE
    std::string pick_slave_default_oco_extra(const std::string& name);
    std::string remove_slave_default_oco_extra();
    std::string pick_slave_champion_oco_extra(const std::string& name);
    std::string remove_slave_champion_oco_extra();

    // => multiple-choice extra selection and removal
    std::string pick_mc_extra(const std::string& name) override;
    std::string remove_mc_extra(const std::string& name) override;
    // ==> MASTER
    std::string pick_master_default_mc_extra(const std::string& name);
    std::string remove_master_default_mc_extra(const std::string& name);
    std::string pick_master_champion_mc_extra(const std::string& name);
    std::string remove_master_champion_mc_extra(const std::string& name);
    // ==> SLAVE
    std::string pick_slave_default_mc_extra(const std::string& name);
    std::string remove_slave_default_mc_extra(const std::string& name);
    std::string pick_slave_champion_mc_extra(const std::string& name);
    std::string remove_slave_champion_mc_extra(const std::string& name);

    // => mount selection and removal
    void pick_mount(const std::string& name) override;
    void remove_mount() override;

    // => banner selection and removal
    std::string pick_banner(ItemCategory item_category, const std::string& name) override;
    std::string remove_banner() override;

    // => command group selection and removal
    void add_command_member(CommandGroup member);
    void remove_command_member(CommandGroup member);
    void add_master_command_member(CommandGroup member);
    void remove_master_command_member(CommandGroup member);
    void add_slave_command_member(CommandGroup member);
    void remove_slave_command_member(CommandGroup member);

    // changing unit size
    void change_size(std::size_t n);
    void change_master_size(std::size_t n);
    void change_slave_size(std::size_t n);

    // serialisation and exporting
    std::string save() const override;
};

#endif // !mixed_unitH
