CONFIG      += qt warn_on release quick-app

HEADERS      = buttonsettings.h \
               buttonutils.h \
               remapdlg.h

SOURCES      = main.cpp \
               buttonsettings.cpp \
               buttonutils.cpp \
               remapdlg.cpp

INTERFACES   = remapdlgbase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
TARGET	     = buttonsettings

include ( $(OPIEDIR)/include.pro )
