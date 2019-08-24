##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------

QT += core
QT -= gui

TARGET = Interfaces
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

include(_out_paths.pri)

INCLUDEPATH += \
    $$SRC_ROOT \
    $$INC_ROOT

SOURCES += \
    ../src/main.cpp \
    ../src/IVector_Impl.cpp \
    ../src/ISet_Impl.cpp

HEADERS += \
    $$INC_ROOT/IBrocker.h \
    $$INC_ROOT/ICompact.h \
    $$INC_ROOT/IProblem.h \
    $$INC_ROOT/ISet.h \
    $$INC_ROOT/ISolver.h \
    $$INC_ROOT/IVector.h \
    $$INC_ROOT/ILog.h \
    $$INC_ROOT/error.h \
    $$INC_ROOT/SHARED_EXPORT.h
