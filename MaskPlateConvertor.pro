QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MaskPlateConvertor
TEMPLATE = app

RESOURCES = images.qrc

*g++*|*clang*{
  QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wformat=2 -Woverloaded-virtual -Wshadow -Wnon-virtual-dtor -Werror
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtCore
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtNetwork
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtGui
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtOpenGL
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/Qt
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT
}


HEADERS += blueprint.h
SOURCES += blueprint.cpp

HEADERS += blueprintview.h
SOURCES += blueprintview.cpp

HEADERS += cpp_extensions.h

SOURCES += main.cpp

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp

HEADERS += program.h
SOURCES += program.cpp

HEADERS += programbasic.h
SOURCES += programbasic.cpp

HEADERS += programcommands.h
SOURCES += programcommands.cpp

HEADERS += programparser.h
SOURCES += programparser.cpp

HEADERS += runningprogram.h
SOURCES += runningprogram.cpp
