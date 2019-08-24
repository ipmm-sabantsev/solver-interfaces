#-------------------------------------------------
#
# Project created by QtCreator 2019-04-28T14:04:47
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = Interfaces
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SRC_ROOT = ../src
INC_ROOT = ../inc
OUT_ROOT = ../../../out

INCLUDEPATH += \
    $$(SRC_ROOT) \
    $$(INC_ROOT)

SOURCES += \
    ../src/main.cpp \

HEADERS += \
    ../inc/IBrocker.h \
    ../inc/ICompact.h \
    ../inc/IProblem.h \
    ../inc/ISet.h \
    ../inc/ISolver.h \
    ../inc/IVector.h \
    ../inc/ILog.h \
    ../inc/error.h \
    ../inc/SHARED_EXPORT.h \
