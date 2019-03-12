#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "enums.h"
#include "magic_item.h"
#include "selection_tree.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
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
#include <QResizeEvent>
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
class option_selector;

class ArmyCreator : public QMainWindow, public std::enable_shared_from_this<ArmyCreator> {
    Q_OBJECT

public:
    static std::shared_ptr<ArmyCreator> create(QWidget* parent = nullptr);
    ~ArmyCreator() override;

public slots:
    void optional_weapon_selected(const std::string& name, WeaponType wt, ItemCategory ic, bool champion, bool master);
    void optional_armour_selected(const std::string& name, ArmourType at, ItemCategory ic, bool champion, bool master);
    void optional_talisman_selected(const std::string& name, ItemCategory ic);
    void optional_enchanted_item_selected(const std::string& name, ItemCategory ic);
    void optional_other_item_selected(const std::string& name, ItemCategory ic, bool checked);
    void optional_banner_selected(const std::string& name, ItemCategory ic);
    void optional_arcane_item_selected(const std::string& name, ItemCategory ic);
    void optional_level_selected(short level);
    void optional_lore_selected(const std::string& name);
    void optional_mount_selected(const std::string& name);
    void optional_mount_oco_extra_selected(const std::string& name);
    void optional_mount_mc_extra_selected(const std::string& name, bool checked);
    void optional_command_selected(CommandGroup member, bool checked, bool master);
    void optional_oco_extra_selected(const std::string& name, bool champion);
    void optional_mc_extra_selected(const std::string& name, bool champion, bool checked);

    void change_unit_size(int value, bool master);
private slots:
    void on_actionExit_triggered();

    void on_faction_combobox_currentTextChanged(const QString& faction);

    void on_points_limit_spinbox_valueChanged(double pts);

    void on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_lord_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_hero_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_core_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_special_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_rare_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_add_button_clicked();
    void on_duplicate_button_clicked();
    void on_remove_button_clicked();
    void on_clear_button_clicked();
    void on_export_button_clicked();

    void on_magic_items_combobox_currentTextChanged(const QString &arg1);
    void on_set_general_button_clicked();
public:
    void update_budget_label();
private:
    explicit ArmyCreator(QWidget *parent = nullptr);
    Ui::ArmyCreator* ui;
    Faction race;
    // handles to the army list and roster
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;
    // handle to currently selected unit
    std::shared_ptr<unit> current;
    int id_counter;
    InTree in_tree;
    // handle to the object which performs all
    // selections and removals of options for units
    std::shared_ptr<option_selector> opt_sel;
    // option boxes
    std::shared_ptr<OptionBox> ob;
    std::shared_ptr<MagicItemBox> mib;
    // directory related fields
    const QString army_list_dir = "WHFB_ArmyLists";
    QString documents_dir;

    QTreeWidgetItem* current_item() const;

    bool army_trees_itemchanged(QTreeWidgetItem* curr);

    void update_validity_label();

    void do_splitter_scalings();
    void initialise_stylesheets();
    void resizeEvent(QResizeEvent* event) override;

    // roster tree modifying
    void populate_roster_tree();
    void clear_roster_tree();

    // army tree modifying
    void resize_army_trees();
    void clear_army_tree();

    // info box modifying
    void clear_unit_info_box();
    void initialise_unit_info_box();

    void clear();
    void clear_points_displays();
    void change_faction(Faction faction);

    void setup_magic_items_combobox();
    void setup_export_directories();

    void update_unit_command_display_helper(
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& command,
        QString& command_str
    );
    void update_character_unit_display(
        QTreeWidgetItem* item,
        CharacterTreeColumn column,
        bool adding,
        bool copying
    );
    void update_noncharacter_unit_display(
        QTreeWidgetItem* item,
        UnitTreeColumn column,
        bool adding,
        bool copying
    );
    void update_unit_display(QTreeWidgetItem* item, ArmyTreeColumn col, bool adding, bool copying);

    void update_unit_displays();

};

#endif // ARMYCREATOR_H
