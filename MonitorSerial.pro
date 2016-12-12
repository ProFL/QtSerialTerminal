#-------------------------------------------------
#
# Project created by QtCreator 2016-12-09T22:10:42
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MonitorSerial
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialport.cpp

HEADERS  += mainwindow.hpp \
    serialport.hpp

FORMS    += mainwindow.ui
