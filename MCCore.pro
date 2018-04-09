QT += core network
QT -= gui

TARGET = MCCore
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    core.cpp

HEADERS += \
    core.h

include(../MCIPC/MCIPC.pri)
