CONFIG		+= qt warn_on release quick-app
HEADERS		= settings.h
SOURCES		= language.cpp main.cpp
INTERFACES	= languagesettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= language

include ( $(OPIEDIR)/include.pro )
