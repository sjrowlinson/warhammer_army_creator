#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "army_maps.h"
#include "magic_item.h"
#include "option_selector.h"
#include "selection_tree.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QSpinBox>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

namespace Ui {
class ArmyCreator;
}

enum class ArmyTreeColumn {
    NAME,
    SIZE,
    WEAPONS,
    ARMOUR,
    COMMAND,
    EXTRAS,
    POINTS,
    ALL
};

class ArmyCreator : public QMainWindow {
    Q_OBJECT

public:
    explicit ArmyCreator(QWidget *parent = nullptr);
    ~ArmyCreator();

private slots:
    void on_actionExit_triggered();

    void on_faction_combobox_currentTextChanged(const QString& faction);

    void on_pts_limit_spinbox_valueChanged(double pts);

    void on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_army_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_add_button_clicked();
    void on_duplicate_button_clicked();
    void on_remove_button_clicked();
    void on_clear_button_clicked();

    void optional_weapon_selected();
    void optional_armour_selected();
    void optional_talisman_selected();
    void optional_enchanted_item_selected();
    void optional_other_item_selected();
    void optional_banner_selected();
    void optional_arcane_item_selected();
    void optional_level_selected();
    void optional_mount_selected();
    void optional_command_selected();
    void optional_oco_extra_selected();
    void optional_mc_extra_selected();

    void change_unit_size();

    void on_magic_items_combobox_currentTextChanged(const QString &arg1);

private:
    Ui::ArmyCreator* ui;
    armies::Faction race;
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;
    int id_counter;
    InTree in_tree;
    ItemClass ic_selected;
    std::shared_ptr<option_selector> opt_sel;

    void update_validity_label();

    // roster tree modifying
    void populate_roster_tree();
    void clear_roster_tree();

    // army tree modifying
    void clear_army_tree();

    void clear_unit_info_box();
    void initialise_unit_info_box();

    // unit options box modifying
    void clear_unit_options_box();
    void initialise_unit_options_box();

    // size and command group option box
    QGroupBox* initialise_size_command_groupbox(std::shared_ptr<unit> current);
    QGroupBox* initialise_command_groupbox(std::shared_ptr<unit> current);
    // weapons option boxes
    std::pair<QGroupBox*, QGroupBox*> initialise_opt_weapons_groupbox(std::shared_ptr<unit> current);
    QGroupBox* initialise_opt_weapons_subgroupbox(WeaponType wt, std::shared_ptr<unit> current, bool champion);
    // armour option boxes
    std::pair<QGroupBox*, QGroupBox*> initialise_opt_armour_groupbox(std::shared_ptr<unit> current);
    QGroupBox* initialise_opt_armour_subgroupbox(ArmourType at, std::shared_ptr<unit> current, bool champion);
    // mount option boxes
    QGroupBox* initialise_opt_mounts_groupbox(std::shared_ptr<unit> current);
    QGroupBox* initialise_opt_mage_levels_groupbox(std::shared_ptr<unit> current);
    // extras option boxes
    std::pair<QGroupBox*, QGroupBox*> initialise_opt_extras_groupbox(std::shared_ptr<unit> current);
    QGroupBox* initialise_opt_oco_extras_groupbox(std::shared_ptr<unit> current, bool champion);
    QGroupBox* initialise_opt_mc_extras_groupbox(std::shared_ptr<unit> current, bool champion);

    void clear_magic_items_selector();
    void init_magic_items_selector(std::shared_ptr<unit> current,
                                   ItemType focus = ItemType::WEAPON);

    QGroupBox* setup_items_tab(const std::unordered_map<std::string, item>& items,
                               std::shared_ptr<unit> current,
                               ItemType item_type);
    QGroupBox* setup_magic_weapons_tab(const std::vector<std::pair<std::string, item>>& items,
                                       std::shared_ptr<unit> current);
    QGroupBox* setup_magic_armour_tab(const std::vector<std::pair<std::string, item>>& items,
                                       std::shared_ptr<unit> current);
    QGroupBox* setup_talismans_tab(const std::vector<std::pair<std::string, item>>& items,
                                   std::shared_ptr<unit> current);
    QGroupBox* setup_enchanted_items_tab(const std::vector<std::pair<std::string, item>>& items,
                                         std::shared_ptr<unit> current);
    QGroupBox* setup_arcane_items_tab(const std::vector<std::pair<std::string, item>>& items,
                                      std::shared_ptr<unit> current);
    QGroupBox* setup_other_items_tab(const std::vector<std::pair<std::string, item>>& items,
                                   std::shared_ptr<unit> current);
    QGroupBox* setup_banners_tab(const std::vector<std::pair<std::string, item>>& items,
                                 std::shared_ptr<unit> current);

    void update_unit_display(
            QTreeWidgetItem* item,
            bool adding,
            ArmyTreeColumn column = ArmyTreeColumn::ALL,
            bool copying = false
    );

};

#endif // ARMYCREATOR_H
