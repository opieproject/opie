CONFIG  = qt quick-app
HEADERS = adpcm.h \
        pixmaps.h \
        helpwindow.h \
        qtrec.h \
        device.h \
        wavFile.h \
        waveform.h
SOURCES = adpcm.c \
        helpwindow.cpp \
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
