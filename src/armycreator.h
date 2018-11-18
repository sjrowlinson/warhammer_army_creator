#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "enums.h"
#include "magic_item.h"
#include "option_selector.h"
#include "selection_tree.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include <QCheckBox>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QString>
#include <QTableWidget>
#include <QTextDocument>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

namespace Ui {
class ArmyCreator;
}

class OptionBox;
class MagicItemBox;

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
    void on_export_button_clicked();

    void optional_weapon_selected();
    void optional_armour_selected();
    void optional_talisman_selected();
    void optional_enchanted_item_selected();
    void optional_other_item_selected();
    void optional_banner_selected();
    void optional_arcane_item_selected();
    void optional_level_selected();
    void optional_mount_selected();
    void optional_mount_oco_extra_selected();
    void optional_mount_mc_extra_selected();
    void optional_command_selected();
    void optional_oco_extra_selected();
    void optional_mc_extra_selected();

    void change_unit_size();

    void on_magic_items_combobox_currentTextChanged(const QString &arg1);
private:
    Ui::ArmyCreator* ui;
    Faction race;
    // handles to the army list and roster
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;
    // handle to currently selected unit (can be in roster or army tree)
    std::shared_ptr<unit> current;
    int id_counter;
    InTree in_tree;
    std::shared_ptr<option_selector> opt_sel;
    std::shared_ptr<OptionBox> ob;
    std::shared_ptr<MagicItemBox> mib;
    // directory related fields
    const QString army_list_dir = "WHFB_ArmyLists";
    QString documents_dir;

    void update_validity_label();

    // roster tree modifying
    void populate_roster_tree();
    void clear_roster_tree();

    // army tree modifying
    void clear_army_tree();

    // info box modifying
    void clear_unit_info_box();
    void initialise_unit_info_box();


    void setup_export_directories();

    void update_unit_command_display_helper(
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& command,
        QString& command_str
    );
    void update_unit_display(
        QTreeWidgetItem* item,
        bool adding,
        ArmyTreeColumn column = ArmyTreeColumn::ALL,
        bool copying = false
    );

};

#endif // ARMYCREATOR_H
