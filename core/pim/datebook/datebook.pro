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
	  noteentryimpl.h \
	  onoteedit.h
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
	  noteentryimpl.cpp \
	  onoteedit.cpp
INTERFACES	= dateentry.ui \
		  datebookdayheader.ui \
		  datebookweekheader.ui \
		  datebookweeklstheader.ui \		  
 		  datebookweeklstdayhdr.ui \		  
 		  repeatentrybase.ui \
		  datebooksettingsbase.ui \
		  noteentry.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET		= datebook

TRANSLATIONS = ../../../i18n/de/datebook.ts \
	 ../../../i18n/xx/datebook.ts \
	 ../../../i18n/en/datebook.ts \
	 ../../../i18n/es/datebook.ts \
	 ../../../i18n/fr/datebook.ts \
	 ../../../i18n/hu/datebook.ts \
	 ../../../i18n/ja/datebook.ts \
	 ../../../i18n/ko/datebook.ts \
	 ../../../i18n/no/datebook.ts \
	 ../../../i18n/pl/datebook.ts \
	 ../../../i18n/pt/datebook.ts \
	 ../../../i18n/pt_BR/datebook.ts \
	 ../../../i18n/sl/datebook.ts \
	 ../../../i18n/zh_CN/datebook.ts \
	 ../../../i18n/it/datebook.ts \
	 ../../../i18n/zh_TW/datebook.ts \
	 ../../../i18n/da/datebook.ts
