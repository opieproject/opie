CONFIG  = qt quick-app
HEADERS = ima_rw.h \
        pixmaps.h \
        qtrec.h \
        device.h \
        wavFile.h \
        waveform.h
SOURCES = ima_rw.c \
        main.cpp \
        qtrec.cpp \
        device.cpp \
        wavFile.cpp \
        waveform.cpp
INTERFACES  =

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lasound
TARGET    = opierec
include( $(OPIEDIR)/include.pro )
