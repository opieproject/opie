TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= mainwindow.h \
		  mainlistview.h \
		  mainlistviewitem.h \
		  maintextview.h \
		  mainwidget.h
SOURCES	= main.cpp \
		  mainwindow.cpp \
		  maintextview.cpp \
		  mainlistview.cpp \
		  mainlistviewitem.cpp \
		  mainwidget.cpp
		  
TARGET		= opieviewer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

