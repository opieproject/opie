TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= mainwindow.h \
		  todotable.h \
		  todoentryimpl.h
SOURCES	= main.cpp \
		  mainwindow.cpp \
		  todotable.cpp \
		  todoentryimpl.cpp

INTERFACES	= todoentry.ui

TARGET		= todolist
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/todolist.ts
TRANSLATIONS += ../i18n/pt_BR/todolist.ts
