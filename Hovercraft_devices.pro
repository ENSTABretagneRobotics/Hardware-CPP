QT -= core
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

TARGET = Hovercraft_devices

DEFINES -= UNICODE
DEFINES += FORCE_MINMAX_DEFINITION
DEFINES += SIGN_DEFINED
DEFINES += SIMULATED_INTERNET_SWARMONDEVICE
DEFINES += DISABLE_MTTHREAD
DEFINES += DISABLE_NMEADEVICETHREAD
DEFINES += DISABLE_SWARMONDEVICETHREAD
DEFINES += DISABLE_SSC32THREAD
win32:DEFINES += WIN32 _CONSOLE

win32:LIBS += -lWS2_32
unix:LIBS += -lrt -lm

SOURCES += \
    Main.cpp \
    OSTime.c \
    OSNet.c \
    OSMisc.c \
    OSCore.c \
    OSComputerRS232Port.c

HEADERS += \
    MT.h \
    NMEADevice.h \
    SwarmonDevice.h \
    SSC32.h \
    RS232Port.h \
    OSTime.h \
    OSNet.h \
    OSMisc.h \
    OSCore.h \
    OSComputerRS232Port.h
