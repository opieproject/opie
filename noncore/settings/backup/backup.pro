CONFIG		= qt warn_on release quick-app
HEADERS		= backuprestore.h
SOURCES		= main.cpp backuprestore.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= backuprestorebase.ui errordialog.ui
TARGET		= backup

include ( $(OPIEDIR)/include.pro )
