CONFIG		+= qt warn on release quick-app

HEADERS		= today.h todaybase.h todayconfig.h
SOURCES		= today.cpp todaybase.cpp todayconfig.cpp main.cpp
INTERFACES = todayconfigmiscbase.ui

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TARGET		= today

include ( $(OPIEDIR)/include.pro )
}
