TEMPLATE	    = app
CONFIG		 = qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = zsafe.h krc2.h category.h categorylist.h zlistview.h \
               scqtfiledlg.h 
SOURCES      = main.cpp zsafe.cpp krc2.cpp category.cpp \
               categorylist.cpp zlistview.cpp shadedlistitem.cpp\
               scqtfileedit.cpp scqtfileedit.moc.cpp \
               scqtfiledlg.cpp
INTERFACES	 = newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui 
INTERFACES  += infoform.ui wait.ui

INCLUDEPATH	+= $(OPIEDIR)/include
INCLUDEPATH	+= .
INCLUDEPATH	+= $(OPIEDIR)/noncore/apps/zsafe
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -Wl,-rpath,$(OPIEDIR)/lib -L$(OPIEDIR)/lib -lqpe -lopie
TARGET		 = zsafe
TRANSLATIONS = $(OPIEDIR)/noncore/apps/zsafe/i18n/de/zsafe.ts

#include ( $(OPIEDIR)/include.pro )

