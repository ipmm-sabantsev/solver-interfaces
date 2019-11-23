##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------

TARGET = Application
TEMPLATE = app

QT += core gui sql qml quick
QMAKE_CXXFLAGS += -std=gnu++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

include(_out_paths.pri)

SRC_APP = $$SRC_ROOT/app/src
INC_APP = $$SRC_ROOT/app/inc
UI_APP  = $$SRC_ROOT/app/ui

DEPENDPATH += \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/log \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/vector \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/compact

LIBS += \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/log    -llog \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/vector -lvector \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/compact -lcompact

INCLUDEPATH += \
    $$INC_ROOT \
    $$INC_APP

SOURCES += \
    $$SRC_APP/connectionwidget.cpp \
    $$SRC_APP/linker.cpp \
    $$SRC_APP/main.cpp \
    $$SRC_APP/qsqlconnectiondialog.cpp \
    $$SRC_APP/solverdialog.cpp \
    $$SRC_APP/sqml.cpp \
    $$SRC_APP/qcustomplot.cpp

HEADERS += \
    $$INC_ROOT/error.h \
    $$INC_ROOT/IBrocker.h \
    $$INC_ROOT/ICompact.h \
    $$INC_ROOT/ILog.h \
    $$INC_ROOT/IProblem.h \
    $$INC_ROOT/ISet.h \
    $$INC_ROOT/ISolver.h \
    $$INC_ROOT/SHARED_EXPORT.h \
    $$INC_ROOT/logging.h

HEADERS += \
    $$INC_APP/connectionwidget.h \
    $$INC_APP/linker.h \
    $$INC_APP/qsqlconnectiondialog.h \
    $$INC_APP/solverdialog.h \
    $$INC_APP/sqml.h \
    $$INC_APP/qcustomplot.h

FORMS += \
    $$UI_APP/linkerwidget.ui \
    $$UI_APP/qsqlconnectiondialog.ui \
    $$UI_APP/solverdialog.ui

DISTFILES += \
    Solver_0.qml
