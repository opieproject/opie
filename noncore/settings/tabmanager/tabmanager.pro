TEMPLATE	= app
CONFIG		= qt warn_on 
HEADERS		= tabmanager.h tabmainwindow.h tablistview.h tabmanagerbase.h tabapplnk.h
SOURCES		= main.cpp tabmanager.cpp tabmanagerbase.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
INTERFACES	= app.ui wait.ui
TARGET		= tabmanager
DESTDIR 	= $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
