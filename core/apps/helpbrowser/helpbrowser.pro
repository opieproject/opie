CONFIG		= qt warn_on release quick-app
HEADERS		= helpbrowser.h magictextbrowser.h
SOURCES		= helpbrowser.cpp magictextbrowser.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	=
TARGET          = helpbrowser

include ( $(OPIEDIR)/include.pro )
