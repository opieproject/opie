TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= snake.h target.h obstacle.h interface.h codes.h
SOURCES		= snake.cpp target.cpp obstacle.cpp interface.cpp main.cpp
TARGET		= snake
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/snake.ts
