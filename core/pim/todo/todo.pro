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

TRANSLATIONS = ../../../i18n/de/todolist.ts \
	 ../../../i18n/en/todolist.ts \
	 ../../../i18n/es/todolist.ts \
	 ../../../i18n/fr/todolist.ts \
	 ../../../i18n/hu/todolist.ts \
	 ../../../i18n/ja/todolist.ts \
	 ../../../i18n/ko/todolist.ts \
	 ../../../i18n/no/todolist.ts \
	 ../../../i18n/pl/todolist.ts \
	 ../../../i18n/pt/todolist.ts \
	 ../../../i18n/pt_BR/todolist.ts \
	 ../../../i18n/sl/todolist.ts \
	 ../../../i18n/zh_CN/todolist.ts \
	 ../../../i18n/it/todolist.ts \
	 ../../../i18n/zh_TW/todolist.ts
