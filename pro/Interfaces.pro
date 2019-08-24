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
CONFIG -= flat

TEMPLATE = vcapp

SOURCES += \
    src/main.cpp \

HEADERS += \
    inc/Broker/IBrocker.h \
    inc/Compact/ICompact.h \
    inc/Problem/IProblem.h \
    inc/Set/ISet.h \
    inc/Solver/ISolver.h \
    inc/Vector/IVector.h \
    inc/Log/ILog.h \
    inc/error.h \
    inc/SHARED_EXPORT.h
