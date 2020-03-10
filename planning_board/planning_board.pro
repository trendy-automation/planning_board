#-------------------------------------------------
#
# Project created by QtCreator 2018-04-23T19:50:20
#
#-------------------------------------------------

include(../../../shared_classes/message_handler/message_handler.pri)
include(../../../shared_classes/single_apprun/single_apprun.pri)
#include(../../../shared_classes/watchdog/watchdog.pri)
include(plc_station/plc_station.pri)
#include(../../../shared_classes/qtxlsx/qtxlsx.pri)
include(udpreceiver/udpreceiver.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared_classes/qtxlsx/build-qtxlsx-Release/release/ -lQtXlsx
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared_classes/qtxlsx/build-qtxlsx-Release/debug/ -lQtXlsx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared_classes/qtxlsx/build-qtxlsx-Release/release/ -lQtXlsx
else:unix: LIBS += -L$$OUT_PWD/../../shared_classes/qtxlsx/build-qtxlsx-Release/ -lQtXlsx

INCLUDEPATH += $$PWD/../../../shared_classes/qtxlsx
DEPENDPATH += $$PWD/../../../shared_classes/qtxlsx

CONFIG(release, debug|release){BUILD_TYPE=release}
CONFIG(debug, debug|release){BUILD_TYPE=debug}

copydata.commands = $(COPY_FILE) \"$$shell_path($$clean_path($$OUT_PWD/../../shared_classes/qtxlsx/build-qtxlsx-Release/$$BUILD_TYPE/QtXlsx.dll))\" \"$$shell_path($$OUT_PWD/$$BUILD_TYPE)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata


DEFINES += DEBUG_NAME=\\\"planning\\\"
DEFINES += APP_NAME='\\"PLANNING_BOARD\\"'
DEFINES += BUILD_PATH='\"$${OUT_PWD}\"'
DEFINES += APP_OPTION_WATHCDOG='\\"watchdog\\"'
DEFINES += APP_OPTION_FORCE='\\"force\\"'
DEFINES += APP_VER='\\"1.0\\"'
DEFINES += FONT_TYPE='\\"Helvetica\\"'
DEFINES += FONT_VALUE=16
DEFINES += DefaultSectionSize=60
DEFINES += COL_PERIOD_SIZE=110
DEFINES += MIN_ROW_HEIGHT=130
#DEFINES += COL_SEBANGO_SIZE=130
#DEFINES += COL_OPERATORS_SIZE=150
DEFINES += COL_ACTUAL_SIZE=100
DEFINES += COL_LOSTTIME_SIZE=100
DEFINES += COL_STATUS_SIZE=100
DEFINES += DATA_FORMAT='\\"dd.MM.yyyy\\"'
DEFINES += TASK_SEPARATOR='\\"|\\"'
DEFINES += CHILDREN_SEPARATOR='\\";\\"'
#PLC UDP connection properties
DEFINES += UDP_PORT='2000'
DEFINES += UDP_PLC_HOST='\\"10.208.105.174\\"'

CONFIG  += c++17

QT      += core gui network widgets concurrent

TARGET = planning_board
TEMPLATE = app
DESTDIR = $${OUT_PWD}/release

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
        ComboBoxDelegate.cpp \
        SpinBoxDelegate.cpp \
        planner.cpp \
        udp_client.cpp

HEADERS += \
        mainwindow.h \
        ComboBoxDelegate.h \
        SpinBoxDelegate.h \
        StyledItemDelegate.h \
        planner.h \
        taskinfo.h \
        udp_client.h \
    GridDelegate.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    ../git_setup.txt

RESOURCES += resources.qrc
