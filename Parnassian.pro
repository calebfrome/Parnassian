#-------------------------------------------------
#
# Project created by QtCreator 2018-04-18T00:10:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Parnassian
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    checklist.cpp \
    observation.cpp

HEADERS  += mainwindow.h \
    checklist.h \
    observation.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource_list.qrc

DISTFILES +=
