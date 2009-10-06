CONFIG  = qt quick-app
HEADERS = pixmaps.h \
        qtrec.h \
        waveform.h
SOURCES = main.cpp \
        qtrec.cpp \
        waveform.cpp
INTERFACES  =

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopiemm2 -lasound
TARGET    = opierec
include( $(OPIEDIR)/include.pro )
