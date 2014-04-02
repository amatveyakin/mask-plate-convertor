QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MaskPlateConvertor
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11


SOURCES += main.cpp

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp
