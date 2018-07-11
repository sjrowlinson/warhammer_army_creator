#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "army_maps.h"
#include "magic_item.h"
#include "selection_tree.h"

#include <functional>
#include <memory>

#include <QMainWindow>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>

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

    void on_add_button_clicked();

private:
    std::unique_ptr<Ui::ArmyCreator> ui;
    armies::Faction race;
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;

    void populate_roster_tree();
    void clear_roster_tree();
    void clear_army_tree();
};

#endif // ARMYCREATOR_H
