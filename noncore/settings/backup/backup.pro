CONFIG		= qt warn_on  quick-app
HEADERS		= backuprestore.h
SOURCES		= main.cpp backuprestore.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2
INTERFACES	= backuprestorebase.ui errordialog.ui
TARGET		= backup
VERSION     = 0.8.1
include( $(OPIEDIR)/include.pro )
