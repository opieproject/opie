CONFIG		+= qt warn_on  quick-app
HEADERS	= datebookday.h \
	  datebook.h \
	  dateentryimpl.h \
	  datebookdayheaderimpl.h \
	  datebooksettings.h \
	  datebookweek.h \
	  modules/weeklst/datebookweeklst.h \
      modules/weeklst/datebookweeklstdayhdr.h \
      modules/weeklst/datebookweeklstheader.h \
      modules/weeklst/datebookweeklstevent.h \
      modules/weeklst/datebookweeklstview.h \
      modules/weeklst/datebookweeklstdblview.h \
      modules/monthview/odatebookmonth.h \
	  datebookweekheaderimpl.h \
	  repeatentry.h \
	  noteentryimpl.h \
	  onoteedit.h \
	  datebookdayallday.h  \
	  namespace_hack.h \
          datebooktypes.h \
          exceptions.h
SOURCES	= main.cpp \
	  datebookday.cpp \
	  datebook.cpp \
	  dateentryimpl.cpp \
	  datebookdayheaderimpl.cpp \
	  datebooksettings.cpp \
	  datebookweek.cpp \
	  modules/weeklst/datebookweeklst.cpp \
      modules/weeklst/datebookweeklstdayhdr.cpp \
      modules/weeklst/datebookweeklstheader.cpp \
      modules/weeklst/datebookweeklstevent.cpp \
      modules/weeklst/datebookweeklstview.cpp \
      modules/weeklst/datebookweeklstdblview.cpp \
      modules/monthview/odatebookmonth.cpp \
	  datebookweekheaderimpl.cpp \
	  repeatentry.cpp \
	  noteentryimpl.cpp \
	  onoteedit.cpp \
	  datebookdayallday.cpp \
      exceptions.cpp
INTERFACES	= dateentry.ui \
		  datebookdayheader.ui \
		  datebookweekheader.ui \
		  modules/weeklst/datebookweeklstheaderbase.ui \
 		  modules/weeklst/datebookweeklstdayhdrbase.ui \
 		  repeatentrybase.ui \
		  datebooksettingsbase.ui \
		  noteentry.ui \
		  exceptionsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopieui2 -lopiecore2 -lopiepim2
TARGET		= datebook

include( $(OPIEDIR)/include.pro )
