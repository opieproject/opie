TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
TARGET		= oyatzee
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

include ( $(OPIEDIR)/include.pro )
