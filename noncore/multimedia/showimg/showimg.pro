TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = $(QPEDIR)/bin

HEADERS		= showimg.h

SOURCES		= main.cpp \
		  showimg.cpp

TARGET          = showimg

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += $(QPEDIR)/include
LIBS            += -lqpe

REQUIRES        = showimg

TRANSLATIONS = ../i18n/de/showimg.ts
