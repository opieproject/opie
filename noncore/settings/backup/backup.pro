CONFIG		= qt warn_on  quick-app
HEADERS		= backuprestore.h
SOURCES		= main.cpp backuprestore.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES	= backuprestorebase.ui errordialog.ui
TARGET		= backup

include ( $(OPIEDIR)/include.pro )
