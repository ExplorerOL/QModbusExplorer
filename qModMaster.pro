#-------------------------------------------------
#
# Project created by QtCreator 2010-11-24T09:57:26
#
#-------------------------------------------------

QT       += core gui

TARGET = qModMaster
TEMPLATE = app

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    3rdparty/libmodbus/modbus.c \
    forms/about.cpp \
    forms/settingsmodbusrtu.cpp \
    forms/settingsmodbustcp.cpp \
    src/modbusadapter.cpp \
    src/eutils.cpp \
    src/registersmodel.cpp \
    src/rawdatamodel.cpp \
    forms/settings.cpp \
    forms/busmonitor.cpp \
    3rdparty/libmodbus/modbus-data.c \
    3rdparty/libmodbus/modbus-tcp.c \
    3rdparty/libmodbus/modbus-rtu.c \
    src/rawdatadelegate.cpp

HEADERS  += src/mainwindow.h \
    3rdparty/libmodbus/modbus.h \
    forms/about.h \
    forms/settingsmodbusrtu.h \
    forms/settingsmodbustcp.h \
    src/modbusadapter.h \
    src/eutils.h \
    src/registersmodel.h \
    src/rawdatamodel.h \
    forms/settings.h \
    forms/busmonitor.h \
    src/rawdatadelegate.h

INCLUDEPATH += 3rdparty/libmodbus

unix:SOURCES +=

unix:DEFINES += _TTY_POSIX_

win32:SOURCES +=

win32:DEFINES += _TTY_WIN_  WINVER=0x0501

win32:LIBS += -lsetupapi -lwsock32 -lws2_32


FORMS    += forms/mainwindow.ui \
    forms/about.ui \
    forms/settingsmodbusrtu.ui \
    forms/settingsmodbustcp.ui \
    forms/settings.ui \
    forms/busmonitor.ui

RESOURCES += \
    data/qModMaster.qrc
