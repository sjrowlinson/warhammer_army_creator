#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "army_maps.h"
#include "magic_item.h"
#include "selection_tree.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QSpinBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

namespace Ui {
class ArmyCreator;
}

enum class InTree {
    ROSTER,
    ARMY,
    NEITHER
};

class ArmyCreator : public QMainWindow {
    Q_OBJECT

public:
    explicit ArmyCreator(QWidget *parent = 0);
    ~ArmyCreator();

private slots:
    void on_actionExit_triggered();

    void on_faction_combobox_currentTextChanged(const QString& faction);

    void on_pts_limit_spinbox_valueChanged(double pts);

    void on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_army_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_add_button_clicked();

    void on_remove_button_clicked();

    void on_clear_button_clicked();

    void optional_weapon_selected();
    void optional_armour_selected();
    void optional_command_selected();

    void change_unit_size();
private:
    Ui::ArmyCreator* ui;
    armies::Faction race;
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;
    int id_counter;
    InTree in_tree;

    // roster tree modifying
    void populate_roster_tree();
    void clear_roster_tree();

    // army tree modifying
    void clear_army_tree();

    // unit options box modifying
    void clear_unit_options_box();
    // TODO: remove from_roster parameters and use in_tree field instead
    void initialise_unit_options_box();
    QGroupBox* init_size_command_groupbox();
    QGroupBox* init_command_groupbox(
                   const std::unordered_map<
                       CommandGroup, std::pair<std::string, double>
                   >& opt_command,
                   const std::unordered_map<
                       CommandGroup, std::pair<std::string, double>
                   >& command
               );

    QGroupBox* init_opt_weapons_groupbox(
            const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& opt_weapons,
            const std::unordered_map<WeaponType, std::tuple<ItemClass, std::string, double>>& weapons,
            int id=0
         );
    QGroupBox* init_opt_armour_groupbox(
            const std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>>& armour,
            int id=0
         );
    QGroupBox* init_opt_mounts_groupbox(
            const std::unordered_map<std::string, std::pair<armies::UnitClass, double>>& mounts,
            int id=0
         );
    QGroupBox* init_opt_extras_groupbox(
            const std::pair<
                std::unordered_map<
                    std::string,
                    std::pair<bool, double>
                >,
                bool
            >& extras,
            int id=0
         );
};

#endif // ARMYCREATOR_H
