CONFIG		+= release
SOURCES		= main.cpp

TARGET		= qss
DESTDIR         = $(OPIEDIR)/bin

INCLUDEPATH    += $(OPIEDIR)/include
DEPENDSPATH    += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
