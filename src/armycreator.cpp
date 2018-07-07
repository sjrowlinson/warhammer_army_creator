#include "armycreator.h"
#include "ui_armycreator.h"

ArmyCreator::ArmyCreator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmyCreator)
{
    ui->setupUi(this);
}

ArmyCreator::~ArmyCreator()
{
    delete ui;
}
