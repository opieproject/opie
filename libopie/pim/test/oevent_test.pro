TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
#HEADERS		=
SOURCES		=  oevent_test.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie 
TARGET		= oevent_test

include ( $(OPIEDIR)/include.pro )