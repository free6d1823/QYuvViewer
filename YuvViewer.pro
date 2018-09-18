#-------------------------------------------------
#
# Project created by QtCreator 2018-09-15T21:52:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YuvViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagewin.cpp

HEADERS  += mainwindow.h \
    imagewin.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
