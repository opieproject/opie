TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= memory.h \
		  graph.h \
		  load.h \
		  storage.h \
		  versioninfo.h \
		  sysinfo.h
SOURCES		= main.cpp \
		  memory.cpp \
		  graph.cpp \
		  load.cpp \
		  storage.cpp \
		  versioninfo.cpp \
		  sysinfo.cpp
INTERFACES	= 

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TARGET		= sysinfo

TRANSLATIONS = ../i18n/de/sysinfo.ts
