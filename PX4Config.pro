#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T22:10:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PX4Config
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    px4_config_file.cpp

HEADERS  += mainwindow.h \
    px4_config_file.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

RC_FILE = icon.rc
