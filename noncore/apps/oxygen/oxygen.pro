TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= oxygen.h \
            kmolcalc.h \
            kmolelements.h \
            calcdlgui.h \
            dataTable.h \
            datawidgetui.h

SOURCES		= main.cpp \
		  oxygen.cpp \
          kmolcalc.cpp \
          calcdlgui.cpp \
          kmolelements.cpp \
          dataTable.cpp \
          datawidgetui.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe
INTERFACES	= calcdlg.ui 
TARGET 		= oxygen
DESTDIR		= $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/oxygen.ts 
