TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(QPEDIR)/bin
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/mindbreaker.ts
