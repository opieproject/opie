TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= interface.h man.h cannon.h base.h bullet.h helicopter.h
SOURCES		= main.cpp interface.cpp man.cpp cannon.cpp base.cpp bullet.cpp helicopter.cpp
TARGET		= parashoot 
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/parashoot.ts
