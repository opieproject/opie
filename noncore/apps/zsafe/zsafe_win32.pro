TEMPLATE	= app
#CONFIG		= qt warn_on thread debug
CONFIG		= qt warn_on thread release
DEFINES		= DESKTOP 
DEFINES += QT_DLL QT_THREAD_SUPPORT
HEADERS         = zsafe.h krc2.h category.h categorylist.h zlistview.h qsettings.h shadedlistitem.h
SOURCES         = main.cpp zsafe.cpp krc2.cpp category.cpp \
                  categorylist.cpp zlistview.cpp qsettings.cpp shadedlistitem.cpp
#INCLUDEPATH	+= $(QTDIR)/include
#INCLUDEPATH	+= .
#DEPENDPATH	+= $(QTDIR)/include
#LIBS            += -lqt
INTERFACES	= newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui
INTERFACES      += infoform.ui wait.ui
TARGET		= zsafe
#DESTDIR        = ../bin
TRANSLATIONS    = i18n/de/zsafe.ts
RC_FILE = zsafe.rc
