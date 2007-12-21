TEMPLATE    = lib
CONFIG      += qt plugin warn_on 

INTERFACES  = configdialogbase.ui \
              promptactiondialog.ui
HEADERS     = cardapplet.h \
              configdialog.h
SOURCES     = cardapplet.cpp \
              configdialog.cpp
TARGET      = cardapplet

DESTDIR     = $(OPIEDIR)/plugins/applets

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lqpe -lopiecore2
VERSION     = 0.9.0

include( $(OPIEDIR)/include.pro )
