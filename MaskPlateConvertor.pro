QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MaskPlateConvertor
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra

*g++*|*clang*{
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtCore
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtNetwork
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtGui
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtOpenGL
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/Qt
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT
}


SOURCES += main.cpp

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp

HEADERS += maskplateprogram.h
SOURCES += maskplateprogram.cpp
