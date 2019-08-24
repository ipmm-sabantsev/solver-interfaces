##--------------------------
## Defines
##--------------------------

include(_defines.pri)

##--------------------------
## Project config
##--------------------------


QT += core
QT -= gui

TARGET = log
CONFIG -= app_bundle

TEMPLATE = lib
CONFIG += dll
DEFINES += \
  "DLL_LIBRARY=1"

include(_out_paths.pri)

INCLUDEPATH += \
    $$INC_ROOT

SOURCES += \
    $$IMP_DIR/Log.cpp

HEADERS += \
    $$INC_ROOT/error.h \
    $$INC_ROOT/SHARED_EXPORT.h \
    $$INC_ROOT/ILog.h
