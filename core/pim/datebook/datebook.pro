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
	  clickablelabel.h

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
	  clickablelabel.cpp

INTERFACES	= dateentry.ui \
		  datebookdayheader.ui \
		  datebooksettingsbase.ui \
		  datebookweekheader.ui \
		  datebookweeklstheader.ui \		  
 		  datebookweeklstdayhdr.ui \		  
 		  repeatentrybase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TARGET		= datebook

TRANSLATIONS = ../i18n/de/datebook.ts
