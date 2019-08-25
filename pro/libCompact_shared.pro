##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------


QT += core
QT -= gui

TARGET = compact
CONFIG -= app_bundle

TEMPLATE = lib
CONFIG += dll
DEFINES += \
  "DLL_LIBRARY=1"

include(_out_paths.pri)

DEPENDPATH += \
  $$OUT_ROOT/$$DBG_RLS_SWITCH/log
  $$OUT_ROOT/$$DBG_RLS_SWITCH/vector
LIBS += \
  -L$$OUT_ROOT/$$DBG_RLS_SWITCH/log    -llog \
  -L$$OUT_ROOT/$$DBG_RLS_SWITCH/vector -lvector

INCLUDEPATH += \
    $$INC_ROOT

SOURCES += \
    $$IMP_DIR/Compact_0.cpp

HEADERS += \
    $$INC_ROOT/error.h \
    $$INC_ROOT/SHARED_EXPORT.h \
    $$INC_ROOT/logging.h \
    $$INC_ROOT/IVector.h \
    $$INC_ROOT/ICompact.h