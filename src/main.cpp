#include "armycreator.h"
#include <QApplication>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ArmyCreator w;
    w.show();
    return a.exec();
}
