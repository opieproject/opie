CONFIG		= qt quick-app
HEADERS		= tabmanager.h tabmainwindow.h tablistview.h tabmanagerbase.h tabapplnk.h
SOURCES		= main.cpp tabmanager.cpp tabmanagerbase.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopiecore2
INTERFACES	= app.ui wait.ui
TARGET		= tabmanager

include ( $(OPIEDIR)/include.pro )
