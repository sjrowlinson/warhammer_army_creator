#include "armycreator.h"
#include <memory>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //auto w = std::make_shared<ArmyCreator>();
    auto w = ArmyCreator::create();
    w->setWindowState(Qt::WindowState::WindowMaximized);
    w->show();
    return a.exec();
}
