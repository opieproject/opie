TEMPLATE	= app
CONFIG		+= qt warn_on quick-app 
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= mainwindow.h \
          bookmanager.h \
          locationmanager.h  \
          show.h \
          view.h \
          descriptionmanager.h  \
          stringmanager.h \
          editor.h \
          managertemplate.h  \
          templatemanager.h \
          datebooksettings.h \
          datebooktypes.h \
          dateentryimpl.h \
          noteentryimpl.h \
          repeatentry.h \
          exceptions.h \
          onoteedit.h \
          holiday.h \
          odatebookaccessbackend_holiday.h \
          namespace_hack.h \
          eventvisualiser.h \
          views/day/dayview.h \
          views/day/datebookday.h \
          views/day/datebookdayheaderimpl.h \
          views/day/datebookdayallday.h \
          views/week/weekview.h \
          views/week/datebookweek.h \
          views/week/datebookweekheaderimpl.h \
          views/weeklst/weeklstview.h \
          views/weeklst/datebookweeklstdblview.h \
          views/weeklst/datebookweeklstdayhdr.h \
          views/weeklst/datebookweeklstheader.h \
          views/weeklst/datebookweeklstview.h \
          views/weeklst/datebookweeklstevent.h \
          views/month/monthview.h \
          views/month/odatebookmonth.h

SOURCES	= main.cpp \
          mainwindow.cpp \
          bookmanager.cpp \
          stringmanager.cpp \
          templatemanager.cpp \
          show.cpp \
          view.cpp \
          editor.cpp \
          datebooksettings.cpp \
          holiday.cpp \
          dateentryimpl.cpp \
          noteentryimpl.cpp \
          repeatentry.cpp \
          exceptions.cpp \
          onoteedit.cpp \
          eventvisualiser.cpp \
          odatebookaccessbackend_holiday.cpp \
          views/day/dayview.cpp \
          views/day/datebookday.cpp \
          views/day/datebookdayheaderimpl.cpp \
          views/day/datebookdayallday.cpp \
          views/week/weekview.cpp \
          views/week/datebookweek.cpp \
          views/week/datebookweekheaderimpl.cpp \
          views/weeklst/weeklstview.cpp \
          views/weeklst/datebookweeklstdayhdr.cpp \
          views/weeklst/datebookweeklstdblview.cpp \
          views/weeklst/datebookweeklstevent.cpp \
          views/weeklst/datebookweeklstheader.cpp \
          views/weeklst/datebookweeklstview.cpp \
          views/month/monthview.cpp \
          views/month/odatebookmonth.cpp

INTERFACES = datebooksettingsbase.ui \
          dateentry.ui \
          noteentry.ui \
          repeatentrybase.ui \
          exceptionsbase.ui \
          views/day/datebookdayheader.ui \
          views/week/datebookweekheader.ui \
          views/weeklst/datebookweeklstheaderbase.ui \
          views/weeklst/datebookweeklstdayhdrbase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe -lopieui2 -lopiecore2 -lopiepim2
TARGET		= datebook

include( $(OPIEDIR)/include.pro )
