CONFIG		+= qt warn on release quick-app

HEADERS		= today.h todaybase.h todayconfig.h
SOURCES		= today.cpp todaybase.cpp todayconfig.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	=
TARGET		= today

include ( $(OPIEDIR)/include.pro )
