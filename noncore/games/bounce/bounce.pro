#CONFIG		= qt warn_on 
CONFIG		= qt warn_on  quick-app
HEADERS		= game.h kbounce.h
SOURCES		= game.cpp kbounce.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
TARGET		= bounce

include ( $(OPIEDIR)/include.pro )
