QT -= core
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

TARGET = Test_devices

DEFINES -= UNICODE
DEFINES += FORCE_MINMAX_DEFINITION
DEFINES += SIGN_DEFINED
DEFINES += DISABLE_THREADS_OSNET
DEFINES += SIMULATED_INTERNET_SWARMONDEVICE
DEFINES += DISABLE_MTTHREAD
DEFINES += DISABLE_RAZORAHRSTHREAD
DEFINES += DISABLE_NMEADEVICETHREAD
DEFINES += DISABLE_SWARMONDEVICETHREAD
DEFINES += DISABLE_P33XTHREAD
DEFINES += DISABLE_SSC32THREAD
DEFINES += DISABLE_MAESTROTHREAD
DEFINES += DISABLE_MINISSCTHREAD
DEFINES += DISABLE_MDMTHREAD
DEFINES += DISABLE_HOKUYOTHREAD
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
    Razor.h \
    NMEADevice.h \
    SwarmonDevice.h \
    P33x.h \
    SSC32.h \
    Maestro.h \
    MiniSSC.h \
    MDM.h \
    Hokuyo.h \
    RS232Port.h \
    OSTime.h \
    OSNet.h \
    OSMisc.h \
    OSCore.h \
    OSComputerRS232Port.h
