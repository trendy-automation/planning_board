#-------------------------------------------------
#
# Project created by QtCreator 2018-04-23T19:50:20
#
#-------------------------------------------------

include(../../../shared_classes/message_handler/message_handler.pri)
include(../../../shared_classes/single_apprun/single_apprun.pri)
#include(../../../shared_classes/excel_class/excel_class.pri)

DEFINES += DEBUG_NAME='\\"planning_board\\"'
DEFINES += APP_NAME='\\"PLANNING_BOARD\\"'
DEFINES += BUILD_PATH='\"$${OUT_PWD}\"'
DEFINES += APP_OPTION_WATHCDOG='\\"watchdog\\"'
DEFINES += APP_OPTION_FORCE='\\"force\\"'
DEFINES += APP_VER='\\"1.0\\"'


CONFIG  += c++17

QT      += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = planning_board
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
        main.cpp \
        mainwindow.cpp \
        ComboBoxDelegate.cpp

HEADERS += \
        mainwindow.h \
        ComboBoxDelegate.h

FORMS += \
        mainwindow.ui
