TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(QPEDIR)/bin

HEADERS	= datebookday.h \
		  datebook.h \
		  dateentryimpl.h \
		  datebookdayheaderimpl.h \
		  datebooksettings.h \
		  datebookweek.h \
		  datebookweekheaderimpl.h \
		  repeatentry.h

SOURCES	= main.cpp \
		  datebookday.cpp \
		  datebook.cpp \
		  dateentryimpl.cpp \
		  datebookdayheaderimpl.cpp \
		  datebooksettings.cpp \
		  datebookweek.cpp \
		  datebookweekheaderimpl.cpp \
		  repeatentry.cpp

INTERFACES	= dateentry.ui \
		  datebookdayheader.ui \
		  datebooksettingsbase.ui \
		  datebookweekheader.ui \
		  repeatentrybase.ui

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TARGET		= datebook

TRANSLATIONS = ../i18n/de/datebook.ts
