CONFIG		+= qt warn on  quick-app

HEADERS		= today.h todaybase.h todayconfig.h
SOURCES		= today.cpp todaybase.cpp todayconfig.cpp main.cpp
INTERFACES = todayconfigmiscbase.ui

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopiepim2 -lopieui2 

TARGET		= today

include ( $(OPIEDIR)/include.pro )
}

