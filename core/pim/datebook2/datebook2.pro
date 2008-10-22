TEMPLATE	= app
CONFIG		+= qt warn_on 
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
          namespace_hack.h \
          views/day/dayview.h \
          views/day/datebookday.h \
          views/day/datebookdayheaderimpl.h \
          views/day/datebookdayallday.h

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
          views/day/dayview.cpp \
          views/day/datebookday.cpp \
          views/day/datebookdayheaderimpl.cpp \
          views/day/datebookdayallday.cpp

INTERFACES = datebooksettingsbase.ui \
          dateentry.ui \
          noteentry.ui \
          repeatentrybase.ui \
          exceptionsbase.ui \
          views/day/datebookdayheader.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe -lopieui2 -lopiecore2 -lopiepim2
TARGET		= datebook2

include( $(OPIEDIR)/include.pro )
