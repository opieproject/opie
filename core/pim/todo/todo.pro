TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= mainwindow.h \
		  todotable.h \
		  todoentryimpl.h \
		  todolabel.h
SOURCES	= main.cpp \
		  mainwindow.cpp \
		  todotable.cpp \
		  todoentryimpl.cpp \
		  todolabel.cc

INTERFACES	= todoentry.ui

TARGET		= todolist
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TRANSLATIONS    = ../i18n/pt_BR/todolist.ts
TRANSLATIONS   += ../i18n/de/todolist.ts
TRANSLATIONS   += ../i18n/en/todolist.ts
TRANSLATIONS   += ../i18n/hu/todolist.ts
TRANSLATIONS   += ../i18n/sl/todolist.ts
TRANSLATIONS   += ../i18n/pl/todolist.ts
TRANSLATIONS   += ../i18n/ja/todolist.ts
TRANSLATIONS   += ../i18n/ko/todolist.ts
TRANSLATIONS   += ../i18n/no/todolist.ts
TRANSLATIONS   += ../i18n/zh_CN/todolist.ts
TRANSLATIONS   += ../i18n/zh_TW/todolist.ts
TRANSLATIONS   += ../i18n/fr/todolist.ts
