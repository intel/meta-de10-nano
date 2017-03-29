#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T06:26:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USB_Gadget
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

desktopInstallPrefix=/opt/$${TARGET}

qtcAddDeployment()
