TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = ../bin

HEADERS		= d3des.h \
		  krfbbuffer.h \
		  krfbcanvas.h \
		  krfbconnection.h \
		  krfbdecoder.h \
		  krfblogin.h \
		  krfboptions.h \
		  krfbserverinfo.h \
		  kvnc.h \
		  kvncconnectdlg.h \
		  kvncoptionsdlg.h \
		  version.h \
		  vncauth.h

SOURCES		= d3des.c \
		  vncauth.c \
		  krfbbuffer.cpp \
		  krfbcanvas.cpp \
		  krfbconnection.cpp \
		  krfbdecoder.cpp \
		  krfblogin.cpp \
		  krfboptions.cpp \
		  kvnc.cpp \
		  kvncconnectdlg.cpp \
		  kvncoptionsdlg.cpp \
		  main.cpp
INTERFACES	= vncoptionsbase.ui
TARGET          = keypebble

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/pt_BR/keypebble.ts
