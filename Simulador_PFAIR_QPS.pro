#-------------------------------------------------
#
# Project created by QtCreator 2017-11-23T10:04:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simulador_PFAIR_QPS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QT += core
QT += widgets
CONFIG += console
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    renderarea.cpp \
    gantt.cpp \
    job.cpp \
    pfair.cpp \
    processador_escalonamento.cpp \
    pfair_old.cpp

HEADERS += \
        mainwindow.h \
    renderarea.h \
    gantt.h \
    job.h \
    pfair.h \
    processador_escalonamento.h \
    pfair_old.h

FORMS += \
        mainwindow.ui
