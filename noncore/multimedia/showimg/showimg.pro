TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin

HEADERS		= showimg.h

SOURCES		= main.cpp \
		  showimg.cpp

TARGET          = showimg

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

REQUIRES        = showimg

TRANSLATIONS = ../i18n/de/showimg.ts
