CONFIG		+= qt warn_on release quick-app
HEADERS		= doctab.h
SOURCES		= doctab.cpp main.cpp
INTERFACES	= doctabsettingsbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= doctab

include ( $(OPIEDIR)/include.pro )
