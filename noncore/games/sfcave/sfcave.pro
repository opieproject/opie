TEMPLATE	= app
CONFIG		+= qt warn_on 
DESTDIR		= $(OPIEDIR)/bin
SOURCES 	= sfcave.cpp helpwindow.cpp random.cpp
HEADERS 	= sfcave.h helpwindow.h random.h
TARGET		= sfcave
INCLUDEPATH 	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe



include ( $(OPIEDIR)/include.pro )
