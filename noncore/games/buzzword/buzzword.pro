CONFIG		= qt warn_on release quick-app
HEADERS		= buzzword.h 
SOURCES		= buzzword.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe

TARGET 		= buzzword

include ( $(OPIEDIR)/include.pro )
