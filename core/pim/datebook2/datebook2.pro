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
          templatemanager.h

SOURCES	= main.cpp \
          mainwindow.cpp \
          bookmanager.cpp \
          stringmanager.cpp \
          templatemanager.cpp \
          show.cpp \
          view.cpp \
	  editor.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET		= datebook2

include ( $(OPIEDIR)/include.pro )
