#-------------------------------------------------
#
# Project created by QtCreator 2018-07-07T01:18:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warhammer_army_creator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
        src/armycreator.cpp \
        src/army_list.cpp \
        src/army_maps.cpp \
        src/item_parser.cpp \
        src/roster_parser.cpp \
        src/selection_tree.cpp \
        src/tools.cpp \
        src/unit.cpp

HEADERS += \
        src/armycreator.h \
        src/army_list.h \
        src/army_maps.h \
        src/base_unit.h \
        src/item_parser.h \
        src/magic_item.h \
        src/roster_parser.h \
        src/selection_tree.h \
        src/tools.h \
        src/unit.h

FORMS += \
        form/armycreator.ui

RESOURCES += \
    rosters.qrc \
    magic_items.qrc
