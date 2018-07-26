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

    void populate_roster_tree();
    void clear_roster_tree();
    void clear_army_tree();
    void clear_unit_options_box();
    void initialise_unit_options_box(bool from_roster);

    // TODO: change signature to return QGroupBox* rather than taking in vbl as param
    void init_size_command_groupbox(
            QVBoxLayout* vbl,
            bool from_roster
         );
    QGroupBox* init_command_groupbox(
                   const std::unordered_map<
                       CommandGroup, std::pair<std::string, double>
                   >& opt_command,
                   const std::unordered_map<
                       CommandGroup, std::pair<std::string, double>
                   >& command,
                   bool from_roster
               );
    // TODO: change signatures to return QGroupBox* rather than taking in vbl as param
    void init_opt_weapons_groupbox(
            QVBoxLayout* vbl,
            const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& opt_weapons,
            const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& weapons,
            bool from_roster,
            int id=0
         );
    void init_opt_armour_groupbox(
            QVBoxLayout* vbl,
            const std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>>& armour,
            bool from_roster,
            int id=0
         );
    void init_opt_mounts_groupbox(
            QVBoxLayout* vbl,
            const std::unordered_map<std::string, std::pair<armies::UnitClass, double>>& mounts,
            bool from_roster,
            int id=0
         );
    void init_opt_extras_groupbox(
            QVBoxLayout* vbl,
            const std::pair<
                std::unordered_map<
                    std::string,
                    std::pair<bool, double>
                >,
                bool
            >& extras,
            bool from_roster,
            int id=0
         );
};

#endif // ARMYCREATOR_H
