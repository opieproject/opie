TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS	= datebookday.h \
	  datebook.h \	
	  dateentryimpl.h \
	  datebookdayheaderimpl.h \
	  datebooksettings.h \
	  datebookweek.h \
	  datebookweeklst.h \
	  datebookweekheaderimpl.h \
	  repeatentry.h \
	  timepicker.h \
	  noteentryimpl.h

SOURCES	= main.cpp \
	  datebookday.cpp \
	  datebook.cpp \
	  dateentryimpl.cpp \
	  datebookdayheaderimpl.cpp \
	  datebooksettings.cpp \
	  datebookweek.cpp \
	  datebookweeklst.cpp \
	  datebookweekheaderimpl.cpp \
	  repeatentry.cpp \
	  timepicker.cpp \
	  noteentryimpl.cpp

INTERFACES	= dateentry.ui \
		  datebookdayheader.ui \
		  datebooksettingsbase.ui \
		  datebookweekheader.ui \
		  datebookweeklstheader.ui \		  
 		  datebookweeklstdayhdr.ui \		  
 		  repeatentrybase.ui \
		  noteentry.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TARGET		= datebook

TRANSLATIONS    = ../i18n/pt_BR/datebook.ts
TRANSLATIONS   += ../i18n/de/datebook.ts
TRANSLATIONS   += ../i18n/en/datebook.ts
TRANSLATIONS   += ../i18n/hu/datebook.ts
TRANSLATIONS   += ../i18n/pl/datebook.ts
TRANSLATIONS   += ../i18n/sl/datebook.ts
TRANSLATIONS   += ../i18n/ja/datebook.ts
TRANSLATIONS   += ../i18n/ko/datebook.ts
TRANSLATIONS   += ../i18n/no/datebook.ts
TRANSLATIONS   += ../i18n/zh_CN/datebook.ts
TRANSLATIONS   += ../i18n/zh_TW/datebook.ts
TRANSLATIONS   += ../i18n/fr/datebook.ts
