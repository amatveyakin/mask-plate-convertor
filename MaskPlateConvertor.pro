QT += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MaskPlateConvertor
TEMPLATE = app

RESOURCES += goldens.qrc
RESOURCES += images.qrc

*g++*|*clang*{
  QMAKE_CXXFLAGS += -std=c++14 -Wall -Wextra -Wformat=2 -Woverloaded-virtual -Wshadow -Wnon-virtual-dtor -Werror
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtCore
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtNetwork
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtGui
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/QtOpenGL
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT/Qt
  QMAKE_CXXFLAGS *= -isystem$$QMAKE_INCDIR_QT
}

#*g++*|*clang*{
#  QMAKE_CXXFLAGS_DEBUG *= -fsanitize=address -fno-omit-frame-pointer
#  QMAKE_LFLAGS_DEBUG *= -fsanitize=address
#}


HEADERS += agilescrollbar.h
SOURCES += agilescrollbar.cpp

HEADERS += appinfo.h
SOURCES += appinfo.cpp

HEADERS += autocadconvertor.h
SOURCES += autocadconvertor.cpp

HEADERS += autocadconvertorsettings.h

HEADERS += autocadsettingsdialog.h
SOURCES += autocadsettingsdialog.cpp

HEADERS += autocadlanguage.h
SOURCES += autocadlanguage.cpp

HEADERS += blueprint.h
SOURCES += blueprint.cpp

HEADERS += blueprintbasic.h
SOURCES += blueprintbasic.cpp

HEADERS += blueprintview.h
SOURCES += blueprintview.cpp

HEADERS += colorutils.h

SOURCES += conversiontest.cpp

HEADERS += forwardmapping.h
SOURCES += forwardmapping.cpp

HEADERS += geometry.h
SOURCES += geometry.cpp

HEADERS += logdatamodel.h
SOURCES += logdatamodel.cpp

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

HEADERS += programtextedit.h
SOURCES += programtextedit.cpp

HEADERS += runningprogram.h
SOURCES += runningprogram.cpp

HEADERS += saveimagedialog.h
SOURCES += saveimagedialog.cpp

HEADERS += searchbar.h
SOURCES += searchbar.cpp

HEADERS += syntaxhighlighter.h
SOURCES += syntaxhighlighter.cpp

HEADERS += textbasic.h
SOURCES += textbasic.cpp

HEADERS += utils.h
SOURCES += utils.cpp
