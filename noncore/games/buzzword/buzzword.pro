TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= buzzword.h 
SOURCES		= buzzword.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
DESTDIR     = $(OPIEDIR)/bin
LIBS        += -lqpe
TARGET		= buzzword 

include ( $(OPIEDIR)/include.pro )
