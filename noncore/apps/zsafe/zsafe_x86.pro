TEMPLATE	= app
# CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release 
DEFINES		= DESKTOP NO_OPIE
HEADERS         = zsafe.h krc2.h category.h categorylist.h zlistview.h shadedlistitem.h
SOURCES         = main.cpp zsafe.cpp krc2.cpp category.cpp categorylist.cpp zlistview.cpp shadedlistitem.cpp
INCLUDEPATH	+= $(QTDIR)/include
INCLUDEPATH	+= .
DEPENDPATH	+= $(QTDIR)/include
INTERFACES	= newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui
INTERFACES      += infoform.ui wait.ui
TARGET		= zsafe
#DESTDIR        = ../bin
TRANSLATIONS    = i18n/de/zsafe.ts
MOC_DIR=.x11
OBJECTS_DIR=.x11

isEmpty(OPIEDIR) {
 message( "Configuring for multi-threaded Qt..." )
}

win32 {
HEADERS+=extra/qsettings.h
SOURCES+=extra/qsettings.cpp
INCLUDEPATH	+= extra
DEPENDPATH	+= extra
}
