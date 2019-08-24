##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------


QT += core
QT -= gui

TARGET = vector
CONFIG -= app_bundle

TEMPLATE = lib
CONFIG += dll
DEFINES += \
  "DLL_LIBRARY=1"

include(_out_paths.pri)

INCLUDEPATH += \
  $$OUT_ROOT/$$DBG_RLS_SWITCH/log
DEPENDPATH += \
  $$OUT_ROOT/$$DBG_RLS_SWITCH/log
LIBS += \
  -L$$OUT_ROOT/$$DBG_RLS_SWITCH/log -llog

INCLUDEPATH += \
    $$INC_ROOT

SOURCES += \
    $$IMP_DIR/Vector_0.cpp

HEADERS += \
    $$INC_ROOT/error.h \
    $$INC_ROOT/SHARED_EXPORT.h \
    $$INC_ROOT/logging.h \
    $$INC_ROOT/IVector.h
