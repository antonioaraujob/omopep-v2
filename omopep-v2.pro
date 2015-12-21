#-------------------------------------------------
#
# Project created by QtCreator 2014-09-15T10:46:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = omopep-v2
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsqlite3

SOURCES += main.cpp\
        mainwindow.cpp \
    simulation.cpp \
    particle.cpp \
    globalrepository.cpp \
    particlerepository.cpp \
    qcustomplot.cpp \
    cell.cpp \
    gridsubinterval.cpp \
    grid.cpp \
    executionthread.cpp \
    emulator.cpp

HEADERS  += mainwindow.h \
    simulation.h \
    particle.h \
    globalrepository.h \
    particlerepository.h \
    qcustomplot.h \
    cell.h \
    gridsubinterval.h \
    grid.h \
    executionthread.h \
    emulator.h


FORMS    += mainwindow.ui
