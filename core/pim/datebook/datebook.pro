CONFIG		+= qt warn_on  quick-app
HEADERS	= datebookday.h \
	  datebook.h \
	  dateentryimpl.h \
	  datebookdayheaderimpl.h \
	  datebooksettings.h \
	  datebookweek.h \
	  datebookweeklst.h \
	  datebookweekheaderimpl.h \
	  repeatentry.h \
	  noteentryimpl.h \
	  onoteedit.h \
	  datebookdayallday.h  \
	  namespace_hack.h
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
	  noteentryimpl.cpp \
	  onoteedit.cpp \
	  datebookdayallday.cpp
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
LIBS            += -lqpe -lopieui2 -lopiecore2 -lopiepim2
TARGET		= datebook

include ( $(OPIEDIR)/include.pro )
