#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release quick-app
HEADERS		= game.h kbounce.h
SOURCES		= game.cpp kbounce.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
TARGET		= bounce

include ( $(OPIEDIR)/include.pro )
