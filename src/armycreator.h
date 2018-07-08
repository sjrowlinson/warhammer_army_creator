#ifndef ARMYCREATOR_H
#define ARMYCREATOR_H

#include <QMainWindow>

namespace Ui {
class ArmyCreator;
}

class ArmyCreator : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArmyCreator(QWidget *parent = 0);
    ~ArmyCreator();

private slots:
    void on_actionExit_triggered();

private:
    Ui::ArmyCreator *ui;
};

#endif // ARMYCREATOR_H
