#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include "army_list.h"
#include "army_maps.h"
#include "selection_tree.h"

#include <functional>
#include <memory>

#include <QMainWindow>
#include <QString>

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

private:
    Ui::ArmyCreator *ui;
    armies::Faction race;
    std::shared_ptr<army_list> army;
    std::shared_ptr<selection_tree> st;
};

#endif // ARMYCREATOR_H
