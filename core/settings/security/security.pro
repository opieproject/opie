CONFIG		+= qt warn_on  quick-app
HEADERS		= security.h
SOURCES		= security.cpp main.cpp
INTERFACES	= securitybase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= security

include ( $(OPIEDIR)/include.pro )
