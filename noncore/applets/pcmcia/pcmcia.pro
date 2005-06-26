TEMPLATE    = lib
CONFIG      += qt plugin warn_on 

INTERFACES  = configdialogbase.ui \
              promptactiondialog.ui
HEADERS     = pcmcia.h \
              configdialog.h
SOURCES     = pcmcia.cpp \
              configdialog.cpp
TARGET      = pcmciaapplet

DESTDIR     = $(OPIEDIR)/plugins/applets

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lqpe -lopiecore2
VERSION     = 0.9.0

include( $(OPIEDIR)/include.pro )
