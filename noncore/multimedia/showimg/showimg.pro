TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = $(QPEDIR)/bin

HEADERS		= showimg.h \
		  settingsdialog.h

SOURCES		= main.cpp \
		  settingsdialog.cpp \
		  showimg.cpp
INTERFACES	= settingsdialogbase.ui

TARGET          = showimg

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += $(QPEDIR)/include
LIBS            += -lqpe

REQUIRES        = showimg

TRANSLATIONS = ../i18n/de/showimg.ts
