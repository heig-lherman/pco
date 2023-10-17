#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T16:05:55
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PCO_Labo_3
TEMPLATE = app

CONFIG += c++17

LIBS += -lpcosynchro

SOURCES += \
    src/mainwindow.cpp \
    src/main.cpp \
    src/mythread.cpp \
    src/threadmanager.cpp
HEADERS  += \
    src/mainwindow.h \
    src/mythread.h \
    src/threadmanager.h
FORMS    += \
    ui/mainwindow.ui
