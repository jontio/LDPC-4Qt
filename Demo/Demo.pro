#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T09:42:29
#
#-------------------------------------------------

QT       += core gui

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo
TEMPLATE = app

#DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    ../LDPC-4Qt/ldpc4qt.cpp

HEADERS  += mainwindow.h \
    ../LDPC-4Qt/ldpc4qt.h

FORMS    += mainwindow.ui

DISTFILES += \
    LICENSE \
    ../LDPC-4Qt/LICENSE \
    ../LDPC-codes/COPYRIGHT \
    ../LDPC-codes/LGPL \
    ../LDPC-codes/License.txt \
    ../LDPC-codes/README.md \
    README.md \
    screenshot.png \
    LDPC1000x2000SimulationResult.png
