##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------


QT += core
QT -= gui

TARGET = problem
CONFIG -= app_bundle

TEMPLATE = lib
CONFIG += dll
DEFINES += "DLL_LIBRARY=1"

include(_out_paths.pri)

INCLUDEPATH += \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/log \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/vector \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/compact

DEPENDPATH += \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/log \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/vector \
    $$OUT_ROOT/$$DBG_RLS_SWITCH/compact \

LIBS += \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/log -llog \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/vector -lvector \
    -L$$OUT_ROOT/$$DBG_RLS_SWITCH/compact -lcompact

INCLUDEPATH += \
    $$INC_ROOT

SOURCES += \
    $$IMP_DIR/Problem_Linear.cpp

HEADERS += \
    $$INC_ROOT/error.h \
    $$INC_ROOT/SHARED_EXPORT.h \
    $$INC_ROOT/logging.h \
    $$INC_ROOT/ICompact.h \
    $$INC_ROOT/IProblem.h \
    $$INC_ROOT/IBrocker.h \
    $$INC_ROOT/ISolver.h
