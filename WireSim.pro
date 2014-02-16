#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T15:43:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WireSim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wiresim.cpp \
    custommodel.cpp \
    renderarea.cpp

HEADERS  += mainwindow.h \
    wiresim.h \
    custommodel.h \
    renderarea.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    LogicIcons.png

RESOURCES += \
    LogicIcons.qrc
