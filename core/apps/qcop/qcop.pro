TEMPLATE	= app
CONFIG		= qt warn_on 
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= 
SOURCES		= main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 
TARGET		= qcop

include ( $(OPIEDIR)/include.pro )
